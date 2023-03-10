#include <include/rt.h>
#include <math.h>
#include <stdio.h>

extern list* stack;
#define make(res) \
   u1 byte1 = safe_get(code, ++pc, len); \
   u1 byte2 = safe_get(code, ++pc, len); \
   res = (byte1 << 8) | byte2; 

static inline Type as_needed(Type t) {
  switch (t) {
    case BYTE:
    case CHAR:
    case SHORT:
    case BOOL:
    case INT: return INT;
    default: return t;
  }
}

char* except = NULL;
static inline u1 safe_get(u1* buf, u2 index, u2 len) {
  if (index >= len) 
    err("Invalid access into array with index %d when length is %d");
  return buf[index];
}

elem* get_string(string* str) {
  elem* e = GC_MALLOC(sizeof(elem));
  e->t = REF;
  class* c = get_class("java/lang/String");
  method* init = get_method(c, "<init>", "([C)V");
  elem* self = GC_MALLOC(sizeof(elem));
  self->t = REF;
  self->data.ref = new_inst(c);
  elem* arg = GC_MALLOC(sizeof(elem));
  arg->t = ARRAY;
  arg->data.arr = new_array(str->len, 5);
  for (u2 i = 0; i < str->len; i++) {
    elem* el = get(arg->data.arr->data, i);
    el->data.integer = at(str, i);
  }
  frame* ctr = new_frame(init, c->cp, new_str("java/lang/String"));
  set(ctr->lvarray, 0, self);
  set(ctr->lvarray, 1, arg);
  exec(ctr);
  e->data.ref = self->data.ref;
  return e;
}

elem* new_obj(char* cls) {
   class* c = get_class(cls);
   elem* e = GC_MALLOC(sizeof(elem));
   e->t = REF;
   e->data.ref = new_inst(c);
   return e;
}

void throw(char* cls, char* msg) {
  class* ex = get_class("java/lang/Exception");
  string* str = new_str("UNCAUGHT EXCEPTION: ");
  concat(str, cls);
  concat(str, "\nCaused due to : ");
  concat(str, msg);
  elem* self = get_string(str);
  method* exinit = get_method(ex, "<init>", "(Ljava/lang/String;)V");
  frame* exctr = new_frame(exinit, ex->cp, ex->this_class);
  elem* exself = new_obj("java/lang/Exception");
  elem* exarg = GC_MALLOC(sizeof(elem));
  exarg->t = REF;
  exarg->data.ref = self->data.ref;
  set(exctr->lvarray, 0, exself);
  set(exctr->lvarray, 1, exarg);
  exec(exctr);
}

elem* exec(frame* f) {
  dbg("Starting method %s.%s with signature %s", f->class->buf, f->mt->name->buf, f->mt->desc->buf);
  attrs* code_attr = (attrs*) get(f->mt->attrs, 0);
  u1* code = code(code_attr).exec;
  u4 len = code(code_attr).code_len;
  for (i8 pc = 0, instr = 0; pc < len; ) {
    instr = pc;
    //dbg("%d", safe_get(code, pc, len));
    switch (safe_get(code, pc, len)) {
      case 0: break; // nop
      case 1: { // aconst_null
        elem* e = GC_MALLOC(sizeof(elem));
        e->t = REF;
        e->data.ref = NULL;
        push(f, e);
      }
      // iconst_<n>
      case 2:
      case 3:
      case 4: 
      case 5:
      case 6:
      case 7:
      case 8: {
        elem* e = GC_MALLOC(sizeof(elem));
        e->t = INT;
        e->data.integer = code[pc] - 3;
        push(f, e);
        break;
      }
      case 16: {
        elem* e = GC_MALLOC(sizeof(elem));
        e->t = INT;
        e->data.integer = safe_get(code, ++pc, len);
        push(f, e);
        break;
      }
      case 18: { // ldc
        u2 index = safe_get(code, ++pc, len);
        pool_elem* p = get_elem(f->cp, index);
        switch (p->tag) {
          case INTEGER: {
            elem* e = GC_MALLOC(sizeof(elem));
            e->t = INT;
            e->data.integer = p->elem.integer;
            push (f, e);
            break;
          }
          case FLT: {
            elem* e = GC_MALLOC(sizeof(elem));
            e->t = FLOAT;
            e->data.flt = p->elem.flt;
            push(f, e);
            break;
          }
          case STRING: push(f, get_string(get_utf8(f->cp, p->elem.string))); break;
        }
        break;
      }
      case 20: { // ldc2_w
        u2 index;
        make(index);
        pool_elem* p = get_elem(f->cp, index);
        switch (p->tag) {
          case LNG: {
            elem* e = GC_MALLOC(sizeof(elem));
            e->t = LONG;
            e->data.lng = p->elem.lng;
            push (f, e);
            break;
          }
          case DBL: {
            elem* e = GC_MALLOC(sizeof(elem));
            e->t = DOUBLE;
            e->data.dbl = p->elem.dbl;
            push (f, e);
            break;
          }
        }
        break;
      }
      case 21: { // iload
        u2 index = safe_get(code, ++pc, len);
        elem* e = get(f->lvarray, index);
        if (as_needed(e->t) != INT)
         err("iload used but index of local variable array is not int");
        push(f, e);
        break;
      }
      // iload_<n>
      case 26: 
      case 27:
      case 28:
      case 29: {
        u1 delta = code[pc] - 26;
        elem* e = get(f->lvarray, delta);
        if (as_needed(e->t) != INT)
          err("iload_<%d> used but index at local variable array is not int", delta);
        push(f, e);
        break;
      }
      // dload_<n>
      case 38:
      case 39:
      case 40:
      case 41: {
        u1 delta = code[pc] - 38;
        elem* e = get(f->lvarray, delta);
        if (e->t != DOUBLE) 
          err("dload_<%d> used but element at index %d is not a double", delta, delta);
        push(f, e);
        break;
      }
      // aload_<n>
      case 42:
      case 43:
      case 44:
      case 45: {
        u1 delta = code[pc] - 42;
        elem* e = get(f->lvarray, delta);
        if (e->t != REF && e->t != ARRAY) 
          err("aload_<%d> used but element at index %d is not a reference", delta, delta);
        push(f, e);
        break;
      }
      case 50 : { // aaload
        elem* index = pop(f);
        if (index->t != INT) 
          err("aaload used but index is not int");
        elem* ref = pop(f);
        if (ref->t != ARRAY)
          err("aaload used but arrayref is not an array reference");
        array* arr = ref->data.arr;
        if (!arr)
          throw("java.lang.NullPointerException", "Array is null");
        if (index->data.integer >= arr->size) {
          throw("java.lang.ArrayIndexOutOfBoundsException", fmt_str("Array access index %d is equal to or more than array length %d", index->data.integer, arr->size)->buf);
          break;
        }
        push(f, get(arr->data, index->data.integer));
        break;
      }
      
      case 52: { // caload
        elem* index = pop(f);
        if (index->t != INT) 
          err("caload used but index is not int");
        elem* ref = pop(f);
        if (ref->t != ARRAY)
          err("caload used but arrayref is not an array reference");
        array* arr = ref->data.arr;
        if (!arr)
          throw("java.lang.NullPointerException", "Array is null");
        else if (index->data.integer >= arr->size) {
          throw("java.lang.ArrayIndexOutOfBoundsException", fmt_str("Array access index %d is equal to or more than array length %d", index->data.integer, arr->size)->buf);
          break;
        }
        push(f, get(arr->data, index->data.integer));
        break;
      }
      case 54: { // istore
        elem* e = pop(f);
        if (as_needed(e->t) != INT)
          err("istore used but argument is not int");
        u1 index = safe_get(code, ++pc, len);
        set(f->lvarray, index, e);
        break;
      }
      // dstore
      case 57: {
        u1 delta = safe_get(code, ++pc, len);
        elem* e = pop(f);
        if (e->t != DOUBLE) 
          err("dstore(%d) used but stack top is not double", delta);
        set(f->lvarray, delta, e);
        set(f->lvarray, delta + 1, e);
        break;
      }
      // istore_<n>
      case 59:
      case 60:
      case 61:
      case 62: {
        u1 delta = code[pc] - 59;
        elem* e = pop(f);
        if (as_needed(e->t) != INT) 
          err("istore_<%d> used but stack top is not int", delta);
        set(f->lvarray, delta, e);
        break;
      }
      // lstore_<n>
      case 63: 
      case 64:
      case 65:
      case 66: {
        u1 delta = code[pc] - 63;
        elem* e = pop(f);
        if (e->t != LONG) 
          err("lstore_<%d> used but stack top is not long", delta);
        set(f->lvarray, delta, e);
        set(f->lvarray, delta + 1, e);
        break;
      }
      // fstore_<n>
      case 67:
      case 68:
      case 69:
      case 70: {
        u1 delta = code[pc] - 67;
        elem* e = pop(f);
        if (e->t != FLOAT) 
          err("fstore_<%d> used but stack top is not float", delta);
        set(f->lvarray, delta, e);
        break;
      }
      // dstore_<n>
      case 71:
      case 72:
      case 73:
      case 74: {
        u1 delta = code[pc] - 71;
        elem* e = pop(f);
        if (e->t != DOUBLE) 
          err("dstore_<%d> used but stack top is not double", delta);
        set(f->lvarray, delta, e);
        set(f->lvarray, delta + 1, e);
        break;
      }
      // astore_<n>
      case 75:
      case 76:
      case 77:
      case 78: {
        u1 delta = code[pc] - 75;
        elem* e = pop(f);
        if (e->t != REF && e->t != ARRAY) 
          err("astore_<%d> used but stack top is not reference", delta);
        set(f->lvarray, delta, e);
        break;
      }
      case 85: { // castore
        elem* val = pop(f);
        elem* index = pop(f);
        if (index->t != INT) 
          err("caload used but index is not int");
        elem* ref = pop(f);
        if (ref->t != ARRAY)
          err("caload used but arrayref is not an array reference");
        array* arr = ref->data.arr;
        if (!arr)
          throw("java.lang.NullPointerException", "Array is null");
        else if (index->data.integer >= arr->size) {
          throw("java.lang.ArrayIndexOutOfBoundsException", fmt_str("Array access index %d is equal to or more than array length %d", index->data.integer, arr->size)->buf);
          break;
        }
        set(arr->data, index->data.integer, val);
        break;
      }
      case 87: pop(f); break; // pop
      // dup
      case 89: {
        elem* e = get(f->stack, f->stack->len - 1);
        elem* new = GC_MALLOC(sizeof(elem));
        new->t = e->t;
        switch (e->t) {
          case INT: new->data.integer = e->data.integer; break;
          case FLOAT: new->data.flt = e->data.flt; break;
          case LONG: new->data.lng = e->data.lng; break;
          case DOUBLE: new->data.dbl = e->data.dbl; break;
          case REF: new->data.ref = e->data.ref; break;
          case ARRAY: new->data.arr = e->data.arr; break;
        }
        push(f, new);
        break;
      }
      case 90: { // dup_x1
        elem* top = pop(f);
        elem* val = pop(f);
        push(f, top);
        push(f, val);
        push(f, top);
        break; 
      }
      case 96: { // iadd
        elem* val1 = pop(f);
        elem* val2 = pop(f);
        if (as_needed(val1->t) != INT && as_needed(val2->t) != INT)
          err("iadd used but arguments are not ints");
        val1->data.integer += val2->data.integer;
        push(f, val1);
        break;
      }
      case 100: { // isub
        elem* val2 = pop(f);
        elem* val1 = pop(f);
        if (as_needed(val1->t) != INT && as_needed(val2->t) != INT)
          err("isub used but arguments are not ints");
        val1->data.integer -= val2->data.integer;
        push(f, val1);
        break;
      }
      case 104 : { // imul
        elem* val1 = pop(f);
        elem* val2 = pop(f);
        if (as_needed(val1->t) != INT && as_needed(val2->t) != INT)
          err("imul used but arguments are not ints");
        val1->data.integer *= val2->data.integer;
        push(f, val1);
        break;
      }
      case 132: { // iinc
        u1 index = safe_get(code, ++pc, len);
        u1 con = safe_get(code, ++pc, len);
        elem* e = get(f->lvarray, index);
        if (e->t != INT)
          err("iinc used but element at index is not int");
        e->data.integer += (i1) con;
        break;
      }
      case 133: { // i2l
        elem* e = pop(f);
        if (e->t != INT)
          err("i2l used but stack top is not int");
        long l = e->data.integer;
        e->t = LONG;
        e->data.lng = l;
        push(f, e);
        break;
      }
      case 134: { // i2f
        elem* e = pop(f);
        if (e->t != INT)
          err("i2f used but stack top is not int");
        float flt = (float) e->data.integer;
        e->t = FLOAT;
        e->data.flt = flt;
        push(f, e);
        break;
      }
      case 135: { // i2d 
        elem* e = pop(f);
        if (e->t != INT)
          err("i2d used but stack top is not int");
        double d = (double) e->data.integer;
        e->t = DOUBLE;
        e->data.dbl = d;
        push(f, e);
        break;
      }
      case 136: { // l2i
        elem* e = pop(f);
        if (e->t != LONG)
          err("l2i used but stack top is not long");
        int i = (int) e->data.lng;
        e->t = INT;
        e->data.integer = i;
        push(f, e);
        break;
      }
      case 145: // i2b
      case 146: // i2c
      case 147: break; // i2s
      case 151: // dcmpl
      case 152: {  // dcmpg 
        elem* val2 = pop(f);
        elem* val1 = pop(f);
        if (val1->t != DOUBLE || val2->t != DOUBLE)
          err("dcmp used byt arguments are not doubles");
        elem* res = GC_MALLOC(sizeof(elem));
        res->t = INT;
        if (isnan(val1->data.dbl) || isnan(val2->data.dbl)) {
          if (code[pc] == 152)
            res->data.integer = 1;
          else
            res->data.integer = -1;
          push(f, res);
        }
        if (val1->data.dbl > val2->data.dbl)
          res->data.integer = 1;
        else if (val1->data.dbl == val2->data.dbl)
          res->data.integer = 0;
        else 
          res->data.integer = -1;
        push(f, res);
        break;
      }
      case 153: // ifeq
      case 154: // ifne
      case 155: // iflt
      case 156: // ifge
      case 157: // ifgt
      case 158: { // ifle
        u2 delta = code[pc] - 153;
        elem* e = pop(f);
        u1 res = 0;
        switch (delta) {
          case 0: res = e->data.integer == 0; break;
          case 1: res = e->data.integer != 0; break;
          case 2: res = e->data.integer < 0; break;
          case 3: res = e->data.integer <= 0; break;
          case 4: res = e->data.integer > 0; break;
          case 5: res = e->data.integer >= 0; break;
        }
        if (res) {
          i2 offset;
          make(offset);
          instr += offset;
          pc = instr;
          continue;
        }
        pc += 2;
        break;
      }
      case 159: // if_icmpeq
      case 160: // if_icmpne
      case 161: // if_icmplt
      case 162: // if_icmpge
      case 163: // if_icmpgt
      case 164: { // if_icmple
        elem* val2 = pop(f);
        elem* val1 = pop(f);
        if (as_needed(val1->t) != INT || as_needed(val2->t) != INT)
          err("if_icmp used but arguments are not ints");
        u1 res = 0;
        switch (code[pc] - 159) {
          case 0: res = val1->data.integer == val2->data.integer; break;
          case 1: res = val1->data.integer != val2->data.integer; break;
          case 2: res = val1->data.integer < val2->data.integer; break;
          case 3: res = val1->data.integer >= val2->data.integer; break;
          case 4: res = val1->data.integer > val2->data.integer; break;
          case 5: res = val1->data.integer <= val2->data.integer; break;
          default: err("Unreachable");
        }
        if (res) {
          i2 offset;
          make(offset);
          instr += offset;
          pc = instr;
          continue;
        }
        pc += 2;
        break;
      }
      case 167: { // goto
        i2 offset;
        make(offset);
        if ((i2)pc - offset <= 0)
          err("Offset to jump %d causes pc underflow", offset);
        instr += offset;
        pc = instr;
        continue;
      }
      case 172: // ireturn
      case 173: // lreturn
      case 174: // freturn
      case 175: // dreturn
      case 176: // areturn
      case 177: goto end; // return
      case 178: // getstatic 
      case 180: { // getfield
        u2 index;
        make(index);
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != FIELD)
          err("getstatic used but constant pool ref is not valid field ref");
        mfiref_elem* fref = pe->elem.fref;
        //pool_elem* cl = get_elem(f->cp, fref->class);
        class* c = get_class(get_utf8(f->cp, fref->class)->buf);
        pool_elem* nt = get_elem(f->cp, fref->nt);
        if (nt->tag != NTYPE) 
          err("name type index of field ref does not point to valid name type structure");
        ntype_elem* nte = nt->elem.nt;
        field* fe = NULL; 
        if (code[instr] == 178)
          fe = get_field(c, get_utf8(f->cp, nte->name)->buf);
        else {
          elem* ef = pop(f);
          fe = get_inst_field(ef->data.ref, get_utf8(f->cp, nte->name)->buf);
        }
        elem* e = GC_MALLOC(sizeof(elem));
        switch (fe->desc->buf[0]) {
          case 'B': {
            e->t = INT;
            e->data.integer = (code[instr] == 178) ? fe->stat_val.byte : fe->dyn_val.byte;
            break;
          }
          case 'C': {
            e->t = INT;
            e->data.integer = (code[instr] == 178) ? fe->stat_val.chr : fe->dyn_val.chr;
            break;
          }
         case 'S': {
            e->t = INT;
            e->data.integer = (code[instr] == 178) ? fe->stat_val.sht : fe->dyn_val.sht;
            break;
          }
          case 'Z': {
            e->t = INT;
            e->data.integer = (code[instr] == 178) ? fe->stat_val.bool : fe->dyn_val.bool;
            break;
          }
          case 'I': {
            e->t = INT;
            e->data.integer = (code[instr] == 178) ? fe->stat_val.integer : fe->dyn_val.integer;
            break;
          }
          case 'J': {
            e->t = LONG; 
            e->data.lng = (code[instr] == 178) ? fe->stat_val.lng : fe->dyn_val.lng;
            break;
          }
          case 'F': {
            e->t = FLOAT;
            e->data.flt = (code[instr] == 178) ? fe->stat_val.flt : fe->dyn_val.flt;
            break;
          }
          case 'D': {
            e->t = DOUBLE;
            e->data.dbl = (code[instr] == 178) ? fe->stat_val.dbl : fe->dyn_val.dbl;
            break;
          }
          case 'L': {
            e->t = REF;
            e->data.ref = (code[instr] == 178) ? fe->stat_val.refer : fe->dyn_val.refer;
            break;
          }
          case '[': {
            e->t = ARRAY;
            e->data.arr = (code[instr] == 178) ? fe->stat_val.refer : fe->dyn_val.refer;
            break;
          }
        }
        push(f, e);
        break;
      }
      case 181: { // putfield
        u2 index;
        make(index);
        elem* e = pop(f);
        elem* cls = pop(f);
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != FIELD) 
          err("putfield used but element at index is not field reference");
        mfiref_elem* fref = pe->elem.fref;
        inst* c = cls->data.ref;
        pool_elem* nt = get_elem(f->cp, fref->nt);
        if (nt->tag != NTYPE) 
          err("name type index of field ref does not point to valid name type structure");
        ntype_elem* nte = nt->elem.nt; 
        field* fld = get_inst_field(c, get_utf8(f->cp, nte->name)->buf);
        if (!fld)
          err("Field not found %s", get_utf8(f->cp, nte->name)->buf);
        if (!equals(fld->desc, get_utf8(f->cp, nte->desc)->buf))
          err("Field descriptors do not match");
        switch (e->t) {
          case BOOL:
          case SHORT:
          case CHAR:
          case BYTE:
          case INT: {
            switch (fld->desc->buf[0]) {
              case 'B': fld->dyn_val.byte = (i1) e->data.integer; break;
              case 'C': fld->dyn_val.chr = (i2) e->data.integer; break;
              case 'S': fld->dyn_val.sht = (i2) e->data.integer; break;
              case 'Z': fld->dyn_val.bool = (i1) e->data.integer; break;
              case 'I': fld->dyn_val.integer = e->data.integer;
              default: err("Value at stack top not assignable to field");
            }         
            break;
          }
          case LONG: {
            if (fld->desc->buf[0] != 'J')
            fld->dyn_val.lng = e->data.lng;
            break;
          }
          case DOUBLE: {
            if (fld->desc->buf[0] != 'D')
              err("Value at stack top not assignable to field");
            fld->dyn_val.dbl = e->data.dbl;
            break;
          }
          case FLOAT: {
            if (fld->desc->buf[0] != 'F')
              err("Value at stack top not assignable to field");
            fld->dyn_val.flt = e->data.flt;
            break;
          }
          default: {
            if (fld->desc->buf[0] != 'L' && fld->desc->buf[0] != '[')
              err("Value at stack top not assignable to field");
            fld->dyn_val.refer = e->data.ref;
            break;
          }
        } 
        break; 
      }
      case 182: // invokevirtual
      case 183: // invokespecial
      case 184: { // invokestatic
        u2 index;
        make(index);
        elem* self;
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != MREF) 
          err("%s used but element at index is not a method reference", (code[instr] - 182 == 0) ? "invokevirtual" : (code[pc] - 182 == 1) ? "invokespecial" : "invokestatic");
        mfiref_elem* mref = pe->elem.mref;
        class *c = get_class(get_utf8(f->cp, mref->class)->buf);
        pool_elem* nt = get_elem(f->cp, mref->nt);
        if (nt->tag != NTYPE) 
          err("name type index of method ref does not point to valid name type structure");
        ntype_elem* nte = nt->elem.nt;
        method* m = get_method(c, get_utf8(f->cp, nte->name)->buf, get_utf8(f->cp, nte->desc)->buf);
        frame* mt = new_frame(m, c->cp, c->this_class);
        elem* e = NULL;
        if (!mt) {
          e = native_call(f, get_utf8(f->cp, nte->name), 0);
        }
        else {
          if (code[instr] != 184)
            mt->args++; 
          for (u2 i = (code[instr] == 184) ? 0 : 1; i < mt->args; i++) {
            elem* e = pop(f);
            if (e->t == DOUBLE || e->t == LONG) {
              set(mt->lvarray, i, e);
              set(mt->lvarray, i + 1, e);
              i++;
            }
            else 
              set(mt->lvarray, i, e);
          }
          if (code[instr] != 184) {
            elem* arg = pop(f);
            if (arg->t != REF)
              err("invokespecial and invokevirtual need instances");
            if (arg->data.ref == NULL)
              throw("java.lang.NullPointerException", "self is null");
            set(mt->lvarray, 0, arg);
          }
          e = exec(mt);
        }
        if (e) {
          push(f, e);
        } 
        break;
      }
      case 186: { // invokedynamic
        /*class* c = get_class(f->class->buf);
        attrs* bs = NULL;
        for (int i = 0; i < c->attrs->len; i++) {
          bs = get(c->attrs, i);
          if (equals(bs->name, "BootstrapMethods"))
            break;
        }
        if (!bs)
          err("Attribute BootstrapMethods not found");
        u2 index;
        make(index);
        pc += 2;
        pool_elem* p = get_elem(f->cp, index);
        if (p->tag != INVDYN)
          err("constant pool ref is not invoke dynamic info");
        u2 ref = bs->attr.bs.bs_met[p->elem.inv->bmeth].ref;
        pool_elem* handle = get_elem(f->cp, ref);
        if (handle->tag != MHANDLE)
          err("constant pool ref is not method handle");
        pool_elem* elem = get_elem(f->cp, handle->elem.handle->ref_index);
        switch(handle->elem.handle->ref_kind) {
          case 1: case 2: 
          case 3: case 4: {
            if (elem->tag != FIELD)
              err("constant pool ref is not field");
            break;
          }
          case 5: case 6:
          case 7: case 8: {
            if (elem->tag != MREF && elem->tag != IMREF)
              err("constant pool ref is neither method ref nor interface method ref");
            mfiref_elem* func = elem->elem.mref;
            class* met = get_class(get_utf8(f->cp, func->class)->buf);
            //dbg("%s", func->desc);
            break;
          }
          case 9: {
            if (elem->tag != IMREF)
              err("constant pool ref is not an interface method ref");
            break;
          } 
          default: err("Invalid reference kind of method handle");
        }
        
        /*pool_elem* pe = get_elem(f->cp, p->elem.inv->nt);
        if (pe->tag != NTYPE)
          err("constant pool ref is not name type");
        string* name = get_utf8(f->cp, pe->elem.nt->name);
        dbg("Starting dynamic method: %s", name->buf);
        string* desc = get_utf8(f->cp, pe->elem.nt->desc);
        class* c = get_class(f->class->buf);
        
        u2 len = bs->attr.bs.bs_met[p->elem.inv->bmeth].args_len;
        u2* args = bs->attr.bs.bs_met[p->elem.inv->bmeth].args; */
        err("invokedynamic not supported");
        break;
      }
      case 187: { // new
       u2 index;
       make(index);
       pool_elem* pe = get_elem(f->cp, index);
       if (pe->tag != CLASS) 
         err("new used but index does not point to valid constant pool class ref");
       class* super = get_class(get_utf8(f->cp, pe->elem.class)->buf);
       while(1) {
         if (equals(super->super_class, "java/lang/Exception")) {
           string* name = new_str(get_utf8(f->cp, pe->elem.class)->buf);
           replace(name, '/','.');
           except = name->buf;
           break;
         }
         else if (equals(super->super_class, "syslib/VMObj")) {
           break;
         }
         else 
           super = get_class(super->super_class->buf);
       } 
       push(f, new_obj(get_utf8(f->cp, pe->elem.class)->buf));
        break;
      }
      case 188: { // newarray
        elem* e = GC_MALLOC(sizeof(elem));
        elem* size = pop(f);
        if (size->t != INT) 
          err("newarray used but array size is not int");
        e->t = ARRAY;
        e->data.arr = new_array(size->data.integer, safe_get(code, ++pc, len));
        push(f, e);
        break;
      }
      case 190: { // arraylength
        elem* e = pop(f);
        if (e->t != ARRAY) 
          err("arraylength used but stack top is not an array");
        elem* ret = GC_MALLOC(sizeof(elem));
        ret->t = INT;
        ret->data.integer = e->data.arr->size;
        push(f, ret);
        break;
      }
      case 198: // ifnull
      case 199: { // ifnonnull
        elem* e = pop(f);
        if (e->t != REF && e->t != ARRAY)
          err("%s used but argument is not reference", (code[instr] == 198) ? "ifnull" : "ifnonnull");
        u1 res = 0;
        if (code[instr] == 198) 
          res = (e->t == REF) ? e->data.ref == NULL : e->data.arr == NULL;
        else
          res = (e->t == REF) ? e->data.ref != NULL : e->data.arr != NULL;
        if (res) {
          i2 off;
          make(off);
          instr += off;
          pc = instr;
          continue;
        }
        pc += 2;
        break;
      }
      default: err("Unrecognised or unimplemented opcode %d", code[pc]);
    }
    //stack_trace(f);
    pc++;
  }
end:
   dbg("Method %s.%s with descriptor %s finished successfully", f->class->buf, f->mt->name->buf, f->mt->desc->buf);
   if (f->ret == EMPTY) {
     remove_last(stack);
     return NULL;
   }
   else {
     elem* ret = pop(f);
     if (as_needed(f->ret) != as_needed(ret->t))
       err("Return type doesn't match with actual element to return"); 
     remove_last(stack);
     return ret;
   }
}
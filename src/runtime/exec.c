#include <include/rt.h>

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
static inline u1 safe_get(u1* buf, u2 index, u2 len) {
  if (index >= len) 
    err("Invalid access into array with index %d when length is %d");
  return buf[index];
}

elem* exec(frame* f) {
  dbg("Starting method %s.%s with signature %s", f->class->buf, f->mt->name->buf, f->mt->desc->buf);
  attrs* code_attr = (attrs*) get(f->mt->attrs, 0);
  u1* code = code(code_attr).exec;
  u4 len = code(code_attr).code_len;
  for (i8 pc = 0, instr = 0; pc < len; ) {
    instr = pc;
    switch (safe_get(code, pc, len)) {
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
          case STRING: {
            elem* e = GC_MALLOC(sizeof(elem));
            e->t = REF;
            string* str = get_utf8(f->cp, p->elem.string);
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
            push(f, e);
            break;
          }
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
        if (e->t != INT) 
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
        if (e->t != FLOAT) 
          err("dstore_<%d> used but stack top is not double", delta);
        set(f->lvarray, delta, e);
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
        }
        push(f, new);
        break;
      }
      case 167: { // goto
        i2 offset;
        make(offset);
        if ((i2)pc - offset <= 0)
          err("Offset to jump %d causes pc underflow", offset);
        dbg("%d", offset);
        instr += offset;
        pc = instr;
        dbg("%d", pc);
        continue;
      }
      case 177: goto end; // return 
      case 178: { // getstatic
        u2 index;
        make(index);
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != FIELD)
          err("getstatic used but constant pool ref is not valid field ref");
        mfiref_elem* fref = pe->elem.fref;
        pool_elem* cl = get_elem(f->cp, fref->class);
        if (cl->tag != CLASS) 
          err("class index of field ref does not point to valid class");
        class* c = get_class(get_utf8(f->cp, cl->elem.class)->buf);
        pool_elem* nt = get_elem(f->cp, fref->nt);
        if (nt->tag != NTYPE) 
          err("name type index of field ref does not point to valid name type structure");
        ntype_elem* nte = nt->elem.nt;
        field* fe = get_field(c, get_utf8(f->cp, nte->name)->buf);
        elem* e = GC_MALLOC(sizeof(elem));
        switch (fe->desc->buf[0]) {
          case 'B': {
            e->t = INT;
            e->data.integer = fe->stat_val.byte;
            break;
          }
          case 'C': {
            e->t = INT;
            e->data.integer = fe->stat_val.chr;
            break;
          }
         case 'S': {
            e->t = INT;
            e->data.integer = fe->stat_val.sht;
            break;
          }
          case 'Z': {
            e->t = INT;
            e->data.integer = fe->stat_val.bool;
            break;
          }
          case 'I': {
            e->t = INT;
            e->data.integer = fe->stat_val.integer;
            break;
          }
          case 'J': {
            e->t = LONG;
            e->data.lng = fe->stat_val.lng;
            break;
          }
          case 'F': {
            e->t = FLOAT;
            e->data.flt = fe->stat_val.flt;
            break;
          }
          case 'D': {
            e->t = DOUBLE;
            e->data.dbl = fe->stat_val.dbl;
            break;
          }
          case 'L': {
            e->t = REF;
            e->data.ref = fe->stat_val.refer;
            break;
          }
          case '[': {
            e->t = ARRAY;
            e->data.arr = fe->stat_val.refer;
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
              case 'I': fld->dyn_val.integer = e->data.integer; break;
              default: err("Value at stack top not assignable to field");
            }
            break;
          }
          case LONG: {
            if (fld->desc->buf[0] != 'J')
              err("Value at stack top not assignable to field");
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
       // dbg("Here");
        break; 
      }
      case 182: // invokevirtual
      case 183: { // invokespecial
        u2 index;
        make(index);
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != MREF) 
          err("%s used but element at index is not a method reference", (code[pc] - 182 == 0) ? "invokevirtual" : "invokespecial");
        mfiref_elem* mref = pe->elem.mref;
        pool_elem* cl = get_elem(f->cp, mref->class);
        if (cl->tag != CLASS) 
          err("class index of method ref does not point to valid class");
        class* c = get_class(get_utf8(f->cp, cl->elem.class)->buf);
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
          set(mt->lvarray, 0, pop(f));
          for (u2 i = 1; i < mt->args + 1; i++) {
            set(mt->lvarray, i, pop(f));
          }
          e = exec(mt);
        }
        if (e) {
          push(f, e);
        } 
        break;
      }
      case 187: { // new
        u2 index;
        make(index);
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != CLASS) 
          err("new used but index does not point to valid constant pool class ref");
        class* c = get_class(get_utf8(f->cp, pe->elem.class)->buf);
        elem* e = GC_MALLOC(sizeof(elem));
        e->t = REF;
        e->data.ref = new_inst(c);
        push(f, e);
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
      default: err("Unrecognised or unimplemented opcode %d", code[pc]);
    }
    //stack_trace(f);
    pc++;
  }
end:
   dbg("Method %s.%s with descriptor %s finished successfully", f->class->buf, f->mt->name->buf, f->mt->desc->buf);
   if (f->ret == EMPTY)
     return NULL;
   else {
     elem* ret = pop(f);
     if (as_needed(f->ret) != ret->t) 
       err("Return type doesn't match with actual element to return");
     return ret;
   }
}
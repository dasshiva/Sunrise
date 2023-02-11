#include <include/rt.h>

#define make(res) \
   u1 byte1 = safe_get(code, ++pc, len); \
   u1 byte2 = safe_get(code, ++pc, len); \
   res = (byte1 << 8) | byte2; 

static inline u1 safe_get(u1* buf, u2 index, u2 len) {
  if (index >= len) 
    err("Invalid access into array with index %d when length is %d");
  return buf[index];
}

void exec(frame* f) {
  dbg("Starting method %s.%s with signature %s", f->class->buf, f->mt->name->buf, f->mt->desc->buf);
  attrs* code_attr = (attrs*) get(f->mt->attrs, 0);
  u1* code = code(code_attr).exec;
  u4 len = code(code_attr).code_len;
  for (u4 pc = 0; pc < len; ) {
    switch (safe_get(code, pc, len)) {
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
        if (e->t != REF) 
          err("aload_<%d> used but element at index %d is not a rreference", delta, delta);
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
          err("lstore_<%d> used but stack top is not int", delta);
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
          err("dstore_<%d> used but stack top is not float", delta);
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
        if (e->t != REF) 
          err("dstore_<%d> used but stack top is not float", delta);
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
        pc = offset;
        continue;
      }
      case 177: break; // return
      case 183: { // invokespecial
        u2 index;
        make(index);
        pool_elem* pe = get_elem(f->cp, index);
        if (pe->tag != MREF) 
          err("invokespecial used but element at index is not a method reference");
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
        elem* ref = pop(f);
        set(mt->lvarray, 0, ref);
        exec(mt);
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
        e->data.ref = new_obj(c);
        push(f, e);
        break;
      }
      default: err("Unrecognised or unimplemented opcode %d", code[pc]);
    }
    pc++;
  }
  dbg("Method execution finished successfully");
}
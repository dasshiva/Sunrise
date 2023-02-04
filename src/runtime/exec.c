#include <include/rt.h>

static inline u1 safe_get(u1* buf, u2 index, u2 len) {
  if (index >= len) 
    err("Invalid access into array with index %d when length is %d");
  return buf[index];
}

void exec(frame* f) {
  dbg("Starting method %s with signature %s", f->mt->name->buf, f->mt->desc->buf);
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
        }
        break;
      }
      // istore_<n>
      case 59:
      case 60:
      case 61:
      case 62: {
        u1 delta = code[pc] - 59;
        elem* e = get(f->stack, 0);
        if (e->t != INT) 
          err("istore_<%d> used but stack top is not int", delta);
        set(f->lvarray, delta, e);
        break;
      }
      case 167: { // goto
        pc++;
        u2 byte1 = safe_get(code, pc, len);
        pc++;
        u2 byte2 = safe_get(code, pc, len);
        i2 offset = (byte1 << 8) | byte2;
        if ((i2)pc - offset <= 0)
          err("Offset to jump %d causes pc underflow", offset);
        pc = offset;
        continue;
      }
      case 177: break; // return
      default: err("Unrecognised or unimplemented opcode %d", code[pc]);
    }
    pc++;
  }
  dbg("Method execution finished successfully");
}
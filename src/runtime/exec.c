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
      case 167: {
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
      case 177: break;
      default: err("Unrecognised or unimplemented opcode %d", code[pc]);
    }
    pc++;
  }
  dbg("Method execution finished successfully");
}
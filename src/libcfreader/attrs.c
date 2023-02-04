#include <include/libcfreader.h>

list* init_attrs(handle* h, list* cpool, u2 len) {
  if (len == 0) 
    return NULL;
  list* p = new_list();
  for (u2 i = 0; i < len; i++) {
    attrs* at = GC_MALLOC(sizeof(attrs));
    at->name = get_utf8(cpool, get_u2(h));
    if (equals(at->name, "ConstantValue")) {
      skip(h, 4);
      dbg("Found attribute ConstantValue");
      at->attr.const_val = get_u2(h);
      add(p, at);
    }
    else if (equals(at->name, "Code")) {
      skip(h, 4);
      dbg("Found attribute Code");
      at->attr.code.stack = get_u2(h);
      at->attr.code.locals = get_u2(h);
      at->attr.code.code_len = get_u4(h);
      at->attr.code.exec = GC_MALLOC(sizeof(u1) * at->attr.code.code_len);
      for (u4 i = 0; i < at->attr.code.code_len; i++) {
        at->attr.code.exec[i] = get_u1(h);
      }
      at->attr.code.etable_len = get_u2(h);
      if (at->attr.code.etable_len != 0) {
        at->attr.code.etable = GC_MALLOC(sizeof(at->attr.code.etable) * at->attr.code.etable_len);
        for (u4 i = 0; i < at->attr.code.etable_len; i++) {
          at->attr.code.etable[i].start_pc = get_u2(h);
          at->attr.code.etable[i].end_pc = get_u2(h);
          at->attr.code.etable[i].handler_pc = get_u2(h);
           at->attr.code.etable[i].catch_type = get_u2(h);
        }
      }
      u2 at_count = get_u2(h);
      for (u2 i = 0; i < at_count; i++) {
        u2 ind = get_u2(h);
        dbg("Skipping Code attribute %s", get_utf8(cpool, ind)->buf);
        skip(h, get_u4(h));
      }
      add(p, at);
    }
    else if (equals(at->name, "SourceFile")) {
      skip(h, 4);
      at->attr.file = get_utf8(cpool, get_u2(h));
      dbg("Found attribute SourceFile with value %s", at->attr.file->buf);
    }
    else {
      skip(h, get_u4(h));
      dbg("Attribute %s not supported. Skipping", at->name->buf);
    }
  }
  return p;
}

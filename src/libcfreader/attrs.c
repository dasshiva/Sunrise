#include <include/libcfreader.h>
#include <stdlib.h>

list* init_attrs(handle* h, list* cpool, u2 len) {
  if (len == 0) 
    return NULL;
  list* p = new_list();
  for (u2 i = 0; i < len; i++) {
    attrs* at = malloc(sizeof(attrs));
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
      //
    }
    else {
      skip(h, get_u4(h));
      dbg("Attribute %s not supported. Skipping", at->name->buf);
      free(at);
    }
  }
  return p;
}
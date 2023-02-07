#include <include/rt.h>

obj* new_obj(class* c) {
  obj* ret = GC_MALLOC(sizeof(obj));
  ret->class = c->this_class;
  ret->cp = c->cp;
  ret->fields = c->fields;
  for (u2 i = 0; i < c-> fields_count; i++) {
    field* f = get(ret->fields, i);
    if (!f->check_val) 
      field_init(f);
  }
  ret->methods = c->methods;
  return ret;
}
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

array* new_array(i4 size, u1 type) {
  array* ret = GC_MALLOC(sizeof(array));
  ret->size = size;
  ret->data = new_list();
  switch (type) {
    case 4: ret->t = BOOL; break;
    case 5: ret->t = CHAR; break;
    case 6: ret->t = FLOAT; break;
    case 7: ret->t = DOUBLE; break;
    case 8: ret->t = BYTE; break;
    case 9: ret->t = SHORT; break;
    case 10: ret->t = INT; break;
    case 11: ret->t = LONG; break;
    default: ret->t = REF;
  }
  for (int i = 0; i < size; i++) {
    elem* e = GC_MALLOC(sizeof(elem));
    e->t = ret->t;
    switch (e->t) {
      case BOOL:
      case BYTE:
      case SHORT:
      case CHAR:
      case INT: e->data.integer = 0; break;
      case FLOAT: e->data.flt = 0.0f; break;
      case DOUBLE: e->data.dbl = 0.0; break;
      case LONG: e->data.lng = 0; break;
      default: e->data.ref = NULL;
    }
    add(ret->data, e);
  }
  return ret;
}
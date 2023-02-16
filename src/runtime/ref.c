#include <include/rt.h>

inst* new_inst(class* c) {
  inst* ret = GC_MALLOC(sizeof(inst));
  ret->class = c->this_class;
  ret->fields = new_list();
  for(u2 i = 0; i < c->fields_count; i++) {
    field* f = get(c->fields, i);
    if (!f->check_val) {
      field* new = GC_MALLOC(sizeof(field));
      new->flags = f->flags;
      new->name = f->name;
      new->desc = f->desc;
      add(ret->fields, new);
    }
  }
  ret->methods = c->methods;
  return ret;
}

field* get_inst_field(inst* c, char* name) {
 if (c->fields == NULL) 
   err("Class %s has no instance fields but get_inst_field() called", c->class->buf);
  for (u2 i = 0; i < c->fields->len; i++) {
    field* f = get(c->fields, i);
    if (equals(f->name, name))
      return f;
  }
  err("Field %s not found", name);
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
#include <include/libcfreader.h>

static void init_field(list* cp, field* f) {
  if (f->attrs == NULL) {
    if (is(f, STAT)) {
      f->check_val = 1;
      switch(f->desc->buf[0]) { 
        case 'B' : f->stat_val.byte = 0; break; 
        case 'C' : f->stat_val.chr = 0; break;
        case 'D' : f->stat_val.flt = 0.0f; break;
        case 'F' : f->stat_val.dbl = 0.0; break; 
        case 'I' : f->stat_val.integer = 0; break; 
        case 'J' : f->stat_val.lng = 0; break; 
        case 'S' : f->stat_val.sht = 0; break; 
        case 'Z' : f->stat_val.bool = 0; break; 
        default: f->stat_val.refer = NULL; 
      }
    }
    else {
      f->check_val = 0;
    }
  }
  else {
    f->check_val = 1;
    attrs* a = get(f->attrs, 0);
    pool_elem* pe = get_elem(cp, a->attr.const_val);
    switch(f->desc->buf[0]) { 
      case 'B' : {
        if(pe->tag != INTEGER) 
         err("ConstantValue has to be CONSTANT_Integer for byte field %s", f->name->buf);
        f->stat_val.byte = pe->elem.integer;
        break;
      }
      case 'C' : {
        if(pe->tag != INTEGER) 
         err("ConstantValue has to be CONSTANT_Integer for char field %s", f->name->buf);
        f->stat_val.chr = pe->elem.integer;
        break;
      }
      case 'I' : {
        if(pe->tag != INTEGER) 
         err("ConstantValue has to be CONSTANT_Integer for int field %s", f->name->buf);
        f->stat_val.integer = pe->elem.integer;
        break;
      }
      case 'S' : {
        if(pe->tag != INTEGER) 
         err("ConstantValue has to be CONSTANT_Integer for byte field %s", f->name->buf);
        f->stat_val.sht = pe->elem.integer;
        break;
      }
      case 'Z' : {
       if(pe->tag != INTEGER) 
         err("ConstantValue has to be CONSTANT_Integer for boolean field %s", f->name->buf);
        f->stat_val.bool = pe->elem.integer;
        break;
      }
      case 'D' : {
        if(pe->tag != DBL) 
         err("ConstantValue has to be CONSTANT_Integer for double field %s", f->name->buf);
        f->stat_val.dbl = pe->elem.dbl;
        break;
      }
      case 'F' : {
        if(pe->tag != FLT) 
         err("ConstantValue has to be CONSTANT_Integer for float field %s", f->name->buf);
        f->stat_val.flt = pe->elem.flt;
        break;
      }
      case 'J' : {
       if(pe->tag != LNG) 
         err("ConstantValue has to be CONSTANT_Integer for boolean field %s", f->name->buf);
        f->stat_val.byte = pe->elem.integer;
        break;
      }
   }
  }
}

class* new_class(char* file) {
  handle* h = open_file(file);
  if (h == NULL)
    err("Class %s not found", file);
  dbg("Found class %s. Loading class", file);
  return new_class_from_handle(h);
}

class* new_class_from_handle(handle* h) {
  class* c = GC_MALLOC(sizeof(class));
  if (get_u4(h) != 0xCAFEBABE) 
    err("File has invalid magic");
  c->minor = get_u2(h);
  c->major = get_u2(h);
  dbg("Running class file version %d.%d", c->major, c->minor);
  c->cp_len = get_u2(h);
  dbg("Constant pool length is %d", c->cp_len);
  c->cp = new_cpool(h, c->cp_len);
  c->flags = get_u2(h);
  c->this_index = get_u2(h);
  c->this_class = get_utf8(c->cp, c->this_index);
  dbg("This class - %s", c->this_class->buf);
  u2 super = get_u2(h);
  if (super != 0) {
    c->super_class = get_utf8(c->cp, super);
    dbg("Super class - %s", c->super_class->buf);
  }
  else 
    c->super_class = NULL;
  c->ints_count = get_u2(h);
  dbg("Number of interfaces implemented - %d", c->ints_count);
  if (c->ints_count == 0)
    c->ints = NULL;
  else {
    c->ints = GC_MALLOC(sizeof(u2) * c->ints_count);
    for (u2 i = 0; i < c->ints_count; i++) {
      c->ints[i] = get_u2(h);
    }
  }
  c->fields_count = get_u2(h);
  dbg("Number of fields present - %d", c->fields_count);
  c->fields = (c->fields_count == 0) ? NULL : new_list();
  for (u2 i = 0; i < c->fields_count; i++) {
    field* f = GC_MALLOC(sizeof(field));
    f->flags = get_u2(h);
    f->name = get_utf8(c->cp, get_u2(h));
    f->desc = get_utf8(c->cp, get_u2(h));
    f->attrs_count = get_u2(h);
    dbg("Found field %s with descriptor %s and %d attributes", f->name->buf, f->desc->buf, f->attrs_count);
    f->attrs = init_attrs(h, c->cp, f->attrs_count);
    init_field(c->cp, f);
    add(c->fields, f);
  }
  c->mets_count = get_u2(h);
  dbg("Number of methods declared %d", c->mets_count);
  c->methods = new_list();
  for (u2 i = 0; i < c->mets_count; i++) {
    method* m = GC_MALLOC(sizeof(method));
    m->flags = get_u2(h);
    m->name = get_utf8(c->cp, get_u2(h));
    m->desc = get_utf8(c->cp, get_u2(h));
    m->attrs_count = get_u2(h);
    dbg("Found method %s with descriptor %s and %d attributes", m->name->buf, m->desc->buf, m->attrs_count);
    m->attrs = init_attrs(h, c->cp, m->attrs_count);
    add(c->methods, m);
  }
  c->attrs_count = get_u2(h);
  c->attrs = init_attrs(h, c->cp, c->attrs_count);
  return c;
}

method* get_method(class* c, char* name, char* desc) {
  for (u2 i = 0; i < c->mets_count; i++) {
    method* m = get(c->methods, i);
    if (equals(m->name, name) && equals(m->desc, desc)) 
      return m;
  }
  return NULL;
}

field* get_field(class* c, char* name) {
  for (u2 i = 0; i < c->fields_count; i++) {
    field* f = get(c->fields, i);
    if (equals(f->name, name))
      return f;
  }
  return NULL;
}
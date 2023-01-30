#include <include/libcfreader.h>
#include <stdlib.h>

class* new_class(char* file) {
  handle* h = open_file(file);
  class* c = malloc(sizeof(class));
  if (get_u4(h) != 0xCAFEBABE) 
    err("File %s has invalid magic", file);
  c->minor = get_u2(h);
  c->major = get_u2(h);
  dbg("Running class file version %d.%d", c->major, c->minor);
  c->cp_len = get_u2(h);
  dbg("Constant pool length is %d", c->cp_len);
  c->cp = new_cpool(h, c->cp_len);
  c->flags = get_u2(h);
  c->this_class = get_utf8(c->cp, get_u2(h));
  dbg("This class - %s", c->this_class->buf);
  c->super_class = get_utf8(c->cp, get_u2(h));
  dbg("Super class - %s", c->super_class->buf);
  c->ints_count = get_u2(h);
  dbg("Number of interfaces implemented - %d", c->ints_count);
  if (c->ints_count == 0)
    c->ints = NULL;
  else {
    c->ints = malloc(sizeof(u2) * c->ints_count);
    for (u2 i = 0; i < c->ints_count; i++) {
      c->ints[i] = get_u2(h);
    }
  }
  c->fields_count = get_u2(h);
  dbg("Number of fields present - %d", c->fields_count);
  return c;
}

#include <include/libcfreader.h>
#include <include/libutils.h>
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
  return c;
}

#include <include/rt.h>
#include <include/libutils.h>
#include <include/config.h>
#include <include/zip.h>
#include <gc.h>

#define init_list() \
 if(classes == NULL) \
 classes = new_list()
#define SYSLIB_SZ 3
static list* classes = NULL;
static int is_loaded(string* name);
char* syslib[2][SYSLIB_SZ] = {
  { "java/lang/Object", "java/lang/String", NULL}, 
  { "syslib/VMObjExt", "syslib/VMStr", NULL }
};

class* get_class(char* file) {
  init_list();
  int l = is_loaded(new_str(file));
  if (l != -1) 
    return get(classes, l);
  class* c = new_class(file);
  add(classes, c);
  return get(classes, classes->len - 1);
}

int load_jar(char* file) {
  init_list();
  int sys = 0;
  if (ends_with(new_str(file), "syslib.jar")) 
    sys = 1;
  struct zip_t *zip = zip_open(file, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
  if_null(zip, err("File %s not found", file));
  int total = zip_entries_total(zip);
  for (int i = 0; i < total; i++) {
    zip_entry_openbyindex(zip, i);
    string* name = new_str(zip_entry_name(zip));
    if (!ends_with(name, ".class"))
      continue;
    handle h;
    h.size = zip_entry_size(zip);
    h.pos = 0;
    h.data = GC_MALLOC(sizeof(u1) * h.size);
    zip_entry_noallocread(zip, h.data, h.size);
    class* c = new_class_from_handle(&h);
    if (sys) {
      for (int i = 0; i < SYSLIB_SZ; i++) {
        if (equals(c->this_class, syslib[1][i])) {
          dbg("Found class %s to be patched", c->this_class->buf);
          c->this_class = new_str(syslib[0][i]);
          dbg("Patched class");
          add(classes, c);
          break;
        }
      }
      add(classes, c);
    }
    else 
      add(classes, c);
  }
  return 1;
}

static int is_loaded(string* name) {
  if (classes->len == 0)
    return -1;
  int dot = find(name, '.');
  string* subs = substr(name, 0, (dot == -1) ? name->len : (u4) dot);
  for (u4 i = 0; i < classes->len; i++) {
    if (equals(((class*) get(classes, i))->this_class, subs->buf))
      return i;
  }
  return -1;
}
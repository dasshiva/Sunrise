#include <include/rt.h>
#include <include/log.h>
#include <include/config.h>
#include <zip.h>

static list* classes = NULL;
static int is_loaded(string* name);
static const char const* syslib[][] = {
  { "java/lang/Object" }, 
  { "syslib/VMObj" }
};

class* get_class(char* file) {
  if (!classes) {
    classes = new_list();
  }
  int l = is_loaded(new_str(file));
  if (l != -1) 
    return get(classes, l);
  class* c = new_class(file);
  add(classes, c);
  return get(classes, classes->len - 1);
}

int load_syslib(char* file) {
  struct zip_t *zip = zip_open(file, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
  int total = zip_entry_total(zip);
  for (int i = 0; i < total; i++) {
    zip_entry_openbyindex(zip, i);
    string* name = new_str(zip_entry_name(zip));
  }
  return 0;
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
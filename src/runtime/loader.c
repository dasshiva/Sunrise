#include <include/rt.h>

static list* classes = NULL;
static int is_loaded(string* name);

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

static int is_loaded(string* name) {
  if (classes->len == 0)
    return -1;
  int dot = find(name, '.');
  string* subs = substr(name, 0, (dot == -1) ? name->len : (u4) dot);
  for (u4 i = 0; i < classes->len; i++) {
    if (equals(((class*) get(classes, i))->this_class, subs->buf)) {
      free_str(subs);
      return i;
    }
  }
  free_str(subs);
  return -1;
}
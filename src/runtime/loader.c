#include <include/rt.h>
#include <include/libutils.h>
#include <include/config.h>
#include <include/zip.h>
#include <gc.h>

#define init_list() \
 if(classes == NULL) \
 classes = new_list()
#define SYSLIB_SZ 17
#define MPATCH_SZ 1
static list* classes = NULL;
static int is_loaded(string* name);
static char* syslib[2][SYSLIB_SZ] = {
  { "java/lang/Object", "java/lang/String", "java/lang/System", "java/io/PrintStream", "java/lang/Exception", "java/lang/RuntimeException", "java/lang/ArrayIndexOutOfBoundsException", "java/lang/StringIndexOutOfBoundsException", "java/io/InputStream", "java/lang/Math", "java/util/Scanner", "java/lang/ArrayIndexOutOfBoundsException", "java/lang/NullPointerException", "java/lang/Character", "java/lang/Integer", "java/lang/StringBuilder", NULL}, 
  { "syslib/VMObjExt", "syslib/VMStr", "syslib/VMSys", "syslib/VMPrtStream", "syslib/VMEx", "syslib/VMRtEx", "syslib/VMArrIndEx", "syslib/VMStrIndEx", "syslib/VMIStream", "syslib/VMMath", "syslib/VMScan", "syslib/VMArrIndEx", "syslib/VMNullPtrEx", "syslib/VMChar", "syslib/VMInt", "syslib/VMStrBuild", NULL }
};

static string* patch_desc(string* desc, char* replace) {
  string* str = new_empty_str();
  for (u4 i = 0; i < desc->len; i++) {
    char c = at(desc, i);
    switch(c) {
      case 'L': {
        int semi = find(desc, ';');
        string* class = substr(desc, i + 1, semi);
        if (starts_with(class, "syslib")) {
          append(str, c);
          concat(str, replace);
          i = semi - 1;
        }
        else 
           append(str, c);
        break;
      }
      case ')': {
        int dend = find(desc, ')') + 1;
        string* ret = substr(desc, dend, desc->len);
        if (starts_with(ret, "Lsyslib")) {
          append(str, ')');
          append(str, 'L');
          concat(str, replace);
          append(str, ';');
          goto end;
        }
      }
      default: append(str, c);
    }
  }
  
end:
  GC_FREE(desc);
  return str;
}

static void patch(int s, class* c) {
  switch (s) {
    case 15: break;
    default: return;
  }
  for (u2 i = 0; i < c->mets_count; i++) {
    method* m = get(c->methods, i);
    if (is(m, PRIV) || m->desc->len < 7)
      continue;
    dbg("Found method %s %s in patchable class ", m->name->buf, m->desc->buf);
    m->desc = patch_desc(m->desc, syslib[0][s]);
    dbg("Patched descriptor to %s", m->desc->buf);
  }
}

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
          pool_elem* pe = GC_MALLOC(sizeof(pool_elem));
          pe->tag = UTF8;
          pe->elem.utf = new_str(syslib[0][i]);
          set(c->cp, c->this_index - 1, pe);
          dbg("Patched class to %s", c->this_class->buf);
          patch(i, c);
          add(classes, c);
          break;
        }
        else if (equals(c->this_class, "syslib/VMObj") || equals(c->this_class, "syslib/VMThrow")) {
          add(classes, c);
        }
      }
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
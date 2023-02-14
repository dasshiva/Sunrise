#include <rt.h>

#define expect(val, type) \
  if (val->t != type) \
   err("Expected value to be of type %s", #type)
 
static char* natives[] = {
  "hashCode", "println", NULL
};

elem* native_call(frame* inv, string* name, int stat) {
  elem* ret = NULL;
  for (int i = 0; ; i++) {
    char* func = natives[i];
    if (!func) 
      err("Native method %s not found", name->buf);
    if (equals(name, natives[i])) {
      switch (i) {
        case 0: {
          elem* this = pop(inv);
          expect(this, REF);
          ret = GC_MALLOC(sizeof(elem));
          ret->t = INT;
          ret->data.integer = (i4) this->data.ref;
          break;
        }
        case 1: {
          elem* arg = pop(inv);
          pop(inv);
          if (arg->t != REF) 
            err("Argument must be an object of type string");
          inst* c = arg->data.ref;
          field* f = get_inst_field(c, "buffer");
          array* arr = f->dyn_val.refer;
          //dbg("Here");
          string* str = new_empty_str();
          for (u4 i = 0; i < arr->size; i++) {
            elem* e = get(arr->data, i);
            append(str, (char)e->data.integer);
          }
          puts(str->buf);
          break;
        }
      }
      break;
    }
  }
  return ret;
}

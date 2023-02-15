#include <rt.h>
#include <stdio.h>

#define expect(val, type) \
  if (val->t != type) \
   err("Expected value to be of type %s", #type)
 
static char* natives[] = {
  "hashCode", "print", "println", NULL
};

elem* native_call(frame* inv, string* name, int stat) {
  elem* ret = NULL;
  for (int i = 0; ; i++) {
    char* func = natives[i];
    if (!func) 
      err("Native method %s not found", name->buf);
    if (equals(name, natives[i])) {
      dbg("%d", i);
      switch (i) {
        case 0: {
          dbg("Native method java/lang/Object.hashCode() starting");
          elem* this = pop(inv);
          expect(this, REF);
          ret = GC_MALLOC(sizeof(elem));
          ret->t = INT;
          ret->data.integer = (i4) this->data.ref;
          dbg("End method java/lang/Object.hashCode()");
          break;
        }
        case 1: 
        case 2: {
          dbg("Native method java/lang/String.%s starting", (i == 2) ? "println" : "print");
          elem* arg = pop(inv);
          pop(inv);
          if (arg->t != REF) 
            err("Argument must be an object of type string");
          inst* c = arg->data.ref;
          field* f = get_inst_field(c, "buffer");
          array* arr = f->dyn_val.refer;
          string* str = new_empty_str();
          for (u4 i = 0; i < arr->size; i++) {
            elem* e = get(arr->data, i);
            append(str, (char)e->data.integer);
          }
          fprintf(stdout, "%s", str->buf);
          if (i == 2)
            fprintf(stdout, "\n");
          dbg("Native method java/lang/String.%s ended", (i == 2) ? "println" : "print");
          break;
        }
      }
      break;
    }
  }
  return ret;
}

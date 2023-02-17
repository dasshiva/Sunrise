#include <rt.h>
#include <stdio.h>
#include <stdlib.h>

extern list* stack;
extern char* except;
#define expect(val, type) \
  if (val->t != type) \
   err("Expected value to be of type %s", #type)
 
static char* natives[] = {
  "hashCode", "print", "println", "exit", "throwEx", "read", NULL
};

string *tostring(elem* arg) {
  inst* c = arg->data.ref;
  field* f = get_inst_field(c, "buffer");
  array* arr = f->dyn_val.refer;
  string* str = new_empty_str();
  for (u4 i = 0; i < arr->size; i++) {
     elem* e = get(arr->data, i);
     append(str, (char)e->data.integer);
  }
  return str;
}

elem* native_call(frame* inv, string* name, int stat) {
  elem* ret = NULL;
  for (int i = 0; ; i++) {
    char* func = natives[i];
    if (!func) 
      err("Native method %s not found", name->buf);
    if (equals(name, natives[i])) {
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
          fprintf(stdout, "%s", tostring(arg)->buf);
          if (i == 2)
            fprintf(stdout, "\n");
          dbg("Native method java/lang/String.%s ended", (i == 2) ? "println" : "print");
          break;
        }
        case 3: {
          dbg("Native method java/lang/System.exit() started");
          elem* e = pop(inv);
          if (e->t != INT)
            err("Method exit() expects int argument");
          exit(e->data.integer);
          break;
        }
        case 4: {
          if (except) {
            fprintf(stderr, "UNCAUGHT EXCEPTION: %s\n", except);
            fprintf(stderr, "Caused by: ");
          }
          fprintf(stderr, "%s\n\nStack trace:\n", tostring(pop(inv))->buf);
          for (int i = stack->len - 1; i >= 0; i--) {
            frame* f = get(stack, i);
            fprintf(stderr, "  at %s.%s %s\n", f->class->buf, f->mt->name->buf, f->mt->desc->buf);
          }
          exit(1);
          break;
        }
        case 5: {
          pop(inv);
          elem* e = GC_MALLOC(sizeof(elem));
          e->t = INT;
          e->data.integer = getchar();
          push(inv, e);
          break;
        }
      }
      break;
    }
  }
  return ret;
}

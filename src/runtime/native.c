#include <rt.h>

#define expect(val, type) \
  if (val->t != type) \
   err("Expected value to be of type %s", #type)
 
static char* natives[] = {
  "hashCode", NULL
};

elem* native_call(frame* inv, string* name, int stat) {
  elem* this = NULL;
  elem* ret = NULL;
  if(!stat) {
    this = pop(inv);
  }
  for (int i = 0; ; i++) {
    char* func = natives[i];
    if (!func) 
      err("Native method %s not found", name->buf);
    if (equals(name, natives[i])) {
      switch (i) {
        case 0: {
          expect(this, REF);
          ret = GC_MALLOC(sizeof(elem));
          ret->t = INT;
          ret->data.integer = (i4) this->data.ref;
          break;
        }
      }
      break;
    }
  }
  return ret;
}

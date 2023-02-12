#include <rt.h>
#include <dlfcn.h>

#define type_gen(ret, num, ...) \
  typedef ret (*nat##ret##num)(__VA_ARGS__);
#define NAT_INT1 1
type_gen(int, 1, elem*)

static char* func_natint1[] = {
  "hashCode"
};

elem* native_call(frame* inv, string* name) {
  void* lib = dlopen(NULL, RTLD_LAZY);
  if (!lib)
    err("Program reached unreachable point (Can't get handle to main program)");
  void* sym = dlsym(lib, name->buf);
  if (!sym)
    err("Failed to load symbol %s", name->buf);
  if (is(inv->mt, STAT)) {}
  else {
    for (int i = 0; i < NAT_INT1; i++) {
      if (equals(name, func_natint1[i])) {
        natint1 nt = sym;
        i4 ret = nt(pop(inv));
        elem* e = GC_MALLOC(sizeof(elem));
        e->t = INT;
        e->data.integer = ret;
        return e;
      }
    }
  }
  dlclose(lib);
}

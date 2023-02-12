#include <rt.h>
#include <dlfcn.h>

typedef nat_int
void native_call(frame* inv, string* name, string* desc) {
  void* lib = dlopen(NULL, RTLD_LAZY);
  if (!lib)
    err("Program reached unreachable point (Can't get handle to main program)");
  void* sym = dlsym(sym, name->buf);
  if (!sym)
    err("Failed to load symbol %s", name->buf);
}

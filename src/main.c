#include <stdio.h>
#include <include/rt.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>

void handler(int signum) {
  fprintf(stderr, "Segmentation fault ocurred");
  exit(1);
}
int main(int argc, char* argv[]) {
    GC_INIT();
    signal(SIGSEGV, &handler);
    class* c = get_class(argv[1]);
    method* m = get_method(c, "main","()V");
    if (m == NULL) 
      err ("Method main not found");
    frame* f = new_frame(m, c->cp);
    exec(f);
    return 0;
}

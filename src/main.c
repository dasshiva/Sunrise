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
    class* c = get_class("Hello.class");
    frame* f = new_frame(get_method(c, "main", "()V"), c->cp);
    exec(f);
    return 0;
}

#include <stdio.h>
#include <include/libcfreader.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>

void handler(int signum) {
  fprintf(stderr, "Segmentation fault ocurred");
  exit(1);
}
int main(int argc, char* argv[]) {
    signal(SIGSEGV, &handler);
    class* c = new_class("Hello.class");
    return 0;
}

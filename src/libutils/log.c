#include <include/libutils.h>
#include <stdio.h>
#include <include/config.h>
#include <stdlib.h>

void dbg(char* fmt, ...) {
  if (app->debug) {
    fputs("DEBUG: ", stderr);
    va_list arg;
    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
    fputs("\n", stderr);
  }
}

void err(char* fmt, ...) {
  fputs("ERROR: ", stderr);
  va_list arg;
  va_start(arg, fmt);
  vfprintf(stderr, fmt, arg);
  va_end(arg);
  fputs("\n", stderr);
  exit(1);
}

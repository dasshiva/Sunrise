#ifndef _RT_H_
#define _RT_H_

#include "libcfreader.h"

class* get_class(char* file);
typedef enum {
  INT,
  LONG,
  FLOAT,
  DOUBLE,
  REF,
  EMPTY
} Type;

typedef struct {
  Type t;
  void* data;
} elem;

typedef struct {
  void* lvarray;
  method* mt;
  list* cp;
  list* stack;
} frame;
frame* new_frame(method* m, list* cp);
void push(frame* f, void* data);
void* pop(frame* f);

#define code(a) a->attr.code
#endif
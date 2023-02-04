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

typedef union {
  i4 integer;
  
} elem_val;

typedef struct {
  Type t;
  elem_val data;
} elem;

typedef struct {
  void* lvarray;
  method* mt;
  list* cp;
  list* stack;
} frame;
frame* new_frame(method* m, list* cp);
void push(frame* f, elem* data);
void* pop(frame* f);

#define code(a) a->attr.code
#endif
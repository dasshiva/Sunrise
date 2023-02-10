#ifndef _RT_H_
#define _RT_H_

#include "libcfreader.h"

class* get_class(char* file);
int load_jar(char* file);

typedef enum {
  INT,
  LONG,
  FLOAT,
  DOUBLE,
  REF,
  EMPTY
} Type;

typedef struct {
  string* class;
  list* cp;
  list* fields;
  list* methods;
} obj;
obj* new_obj(class* c);

typedef union {
  i4 integer;
  float flt;
  i8 lng;
  double dbl;
  obj* ref;
} elem_val;

typedef struct {
  Type t;
  elem_val data;
} elem;

typedef struct {
  list* lvarray;
  method* mt;
  list* cp;
  list* stack;
} frame;
frame* new_frame(method* m, list* cp);
void push(frame* f, elem* data);
void* pop(frame* f);

#define code(a) a->attr.code
#endif
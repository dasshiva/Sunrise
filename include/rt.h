#ifndef _RT_H_
#define _RT_H_

#include "libcfreader.h"

class* get_class(char* file);
typedef enum {
  INT,
  LONG,
  FLOAT,
  DOUBLE,
  REF
} Type;

typedef struct {
  Type t;
  void* data;
} elem;

typedef struct f{
  void* lvarray;
  method* mt;
  list* cp;
  list* stack;
} frame;

#endif
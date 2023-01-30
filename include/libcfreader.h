#ifndef _LIBCFREADER_H_
#define _LIBCFREADER_H_

#include "types.h"
typedef struct {
  u1* data;
  u8 size;
  u8 pos;
} handle;

handle* open_file(char* name);
u1 get_u1(handle* h);
u2 get_u2(handle* h);
u4 get_u4(handle* h);

typedef struct {
  
} utf8_elem;

typedef struct {
  u1 tag;
  union {
    utf8_elem* utf;
  } elem;
} pool_elem;

typedef struct {
  pool_elem* elems;
} pool;

typedef struct {
  u2 minor;
  u2 major;
  u2 cp_len;
  pool* cp;
} class;
class* new_class(char* name);
#endif
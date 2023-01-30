#ifndef _LIBCFREADER_H_
#define _LIBCFREADER_H_

#include "types.h"
#include "libutils.h"
typedef struct {
  u1* data;
  u8 size;
  u8 pos;
} handle;

handle* open_file(char* name);
u1 get_u1(handle* h);
u2 get_u2(handle* h);
u4 get_u4(handle* h);

#define UTF8 1 //  CONSTANT_Utf8
#define CLASS 7 // CONSTANT_Class
#define STRING 8 // CONSTANT_String
#define FIELD 9 // CONSTANT_FieldRef
#define MREF 10 // CONSTANT_MethodRef
#define NTYPE 12 // CONSTANT_NameAndType
#define MTYPE 16 // CONSTANT_MethodType

typedef struct {
  u2 length;
  string* data;
} utf8_elem;

// Represents a field, method or interface method ref
typedef struct {
  u2 class;
  u2 nt;
} mfiref_elem;

typedef struct {
  u1 tag;
  union {
    utf8_elem* utf;
    mfiref_elem* mref; // method reference 
    mfiref_elem* fref; // field reference 
    mfiref_elem* iref; // interface method reference 
  } elem;
} pool_elem;

typedef struct {
  list* elems;
} cpool;
cpool* new_cpool(handle* h, u2 len);

typedef struct {
  u2 minor;
  u2 major;
  u2 cp_len;
  cpool* cp;
} class;
class* new_class(char* name);
#endif
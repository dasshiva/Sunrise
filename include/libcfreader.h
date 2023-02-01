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
void skip(handle* h, u4 amt);

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

typedef struct {
  u2 name;
  u2 desc;
} ntype_elem;
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
    u2 class; // index representing class name;
    ntype_elem* nt;
  } elem;
} pool_elem;

list* new_cpool(handle* h, u2 len);
string* get_utf8(list* pool, u2 index);

typedef struct {
  string* name;
  union {
    u2 const_val;
    struct {
      u2 stack;
      u2 locals;
      u4 code_len;
      u1* exec;
      u2 etable_len;
      struct {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
      } *etable;
    } code;
    string* file;
  } attr;
} attrs;
list* init_attrs(handle* h, list* cpool, u2 len);

typedef struct {
  u2 flags;
  string* name;
  string* desc;
  u2 attrs_count;
  list* attrs;
} field;
typedef field method;

typedef struct {
  u2 minor;
  u2 major;
  u2 cp_len;
  list* cp;
  u2 flags;
  string* super_class;
  string* this_class;
  u2 ints_count;
  u2* ints;
  u2 fields_count;
  list* fields;
  u2 mets_count;
  list* methods;
  u2 attrs_count;
  list* attrs;
} class;
class* new_class(char* name);

#define PUB 0x0001
#define PRIV 0x0002
#define PROT 0x0004
#define STAT 0x0008
#define FINAL 0x0010
#define SYNC 0x0020
#define VOL 0x0040
#define VAR 0x0080
#define TRANS 0x0080
#define NATIVE 0x0100
#define INTER 0x0200
#define ABS 0x0400
#define ANNOT 0x2000
#define ENUM 0x4000

#define is(c, f) ((c->flags & f) != 0)
#endif

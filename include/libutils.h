#ifndef _LIBUTILS_H_
#define _LIBUTILS_H_

#include "types.h"
#include <stdarg.h>

typedef struct {
  u4 len;
  u4 cap;
  char* buf;
} string;
string* new_str(const char* str);
string* new_empty_str(void);
string* str_with_len(u4 len);
void append(string* s, char c);
char at(string *s, u4 index);
int equals(const string* lhs, const char* rhs);
void concat(string* dest, const string* src);
string* substr(string* s, const u4 start, const u4 end);

typedef struct __list {
  u4 len;
  u4 index;
  void* data;
  struct __list* next;
  struct __list* prev;
} list;
list* new_list(void);
void add(list* l, void* data);
void* get(list* l, u4 index);
void delete(list* l, u4 index);

#define is_null(a) a == NULL
void dbg(char* fmt, ...);
void err(char* fmt, ...);

#endif

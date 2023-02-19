#ifndef _LIBUTILS_H_
#define _LIBUTILS_H_

#include <gc.h>
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
void concat(string* dest, const char* src);
string* substr(string* s, const u4 start, const u4 end);
int find(string* s, char c);
int starts_with(const string* s, const char* str);
int ends_with(const string* s, const char* str);
void cat_start(string* dest, const char* str);
void replace(string* str, char c, char new);
string* fmt_str(char* fmt, ...);

typedef struct __node {
  u4 index;
  void* data;
  struct __node* next;
} node;

typedef struct {
  u4 len;
  node* head;
} list;

list* new_list(void);
void add(list* l, void* data);
void* get(list* l, u4 index);
void set(list* l, u4 index, void* data);
void remove_last(list* l);

#define if_null(val, action) \
  if (val == NULL) \
  action;
#define is_null(val) (val == NULL)
void dbg(char* fmt, ...);
void err(char* fmt, ...);

#endif

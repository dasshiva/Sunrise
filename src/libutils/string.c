#include <include/libutils.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

string* new_str(const char* str) {
  string* s = malloc(sizeof(string));
  s->len = strlen(str);
  s->cap = s->len + 20;
  s->buf = malloc(sizeof(char) * s->cap);
  strcpy(s->buf, str);
  s->buf[s->len] = '\0';
  return s;
}

int equals(const string* lhs, const string* rhs) {
  if (lhs->len == rhs->len) {
    u4 a = 0;
    while (a < lhs->len) {
      if (lhs->buf[a] != rhs->buf[a]) 
        return 0;
    }
    return 1;
  }
  
  return 0;
}

void concat(string* dest, const string* src) {
  if (dest->cap + 1 < dest->len + src->len) {
    dest->cap = dest->len + src->len + 20;
    dest->buf = realloc(dest->buf, dest->cap);
  }
  strcat(dest->buf, src->buf);
  dest->len += src->len;
  dest->buf[dest->len] = '\0';
}

string* substr(string* s, const u4 start, const u4 end) {
  string* str = malloc(sizeof(string));
  str->len = end - start;
  str->cap = str->len + 20;
  str->buf = malloc(sizeof(char) * str->cap);
  for (u4 i = start, j = 0; i < end; i++, j++) {
    str->buf[j] = s->buf[i];
  }
  return s;
}

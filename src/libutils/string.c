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

string* new_empty_str(void) {
  return new_str("");
}

string* str_with_len(u4 len) {
  string* s = new_empty_str();
  if (s->cap > len) 
    s->buf = realloc(s->buf, len);
  return s;
}

void append(string* s, char c) {
  if (s->len == s->cap) {
    s->buf = realloc(s->buf, s->cap + 20);
    s->cap += 20;
  }
  s->buf[s->len] = c;
  s->buf[s->len + 1] = '\0';
  s->len++;
}

int equals(const string* lhs, const char* rhs) {
  if (lhs->len == strlen(rhs)) {
    u4 a = 0;
    while (a < lhs->len) {
      if (lhs->buf[a] != rhs[a]) 
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
  if (start >= end) 
    err("Trying to extract sub string when start index(%d) is more than end index(%d)", start, end);
  string* str = malloc(sizeof(string));
  str->len = end - start;
  str->cap = str->len + 20;
  str->buf = malloc(sizeof(char) * str->cap);
  for (u4 i = start, j = 0; i < end; i++, j++) {
    str->buf[j] = s->buf[i];
  }
  return s;
}

char at(string *s, u4 index) {
  if (index == s->len) 
    err("Index %d is invalid for string of length %d", index, s->len);
  return s->buf[index];
}

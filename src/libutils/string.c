#include <include/libutils.h>
#include <string.h>
#include <stdlib.h>

string* new_str(const char* str) {
  string* s = GC_MALLOC(sizeof(string));
  s->len = strlen(str);
  s->cap = s->len + 20;
  s->buf = GC_MALLOC(sizeof(char) * s->cap);
  strcpy(s->buf, str);
  s->buf[s->len] = '\0';
  return s;
}

string* new_empty_str(void) {
  return new_str("");
}

string* fmt_str(char* fmt, ...) {
  string* s = new_empty_str();
  char* buf[20];
  va_list ap;
  va_start(ap, fmt);
  for (u4 i = 0; i < strlen(fmt); i++) {
    if (fmt[i] != '%') 
      append(s, fmt[i]);
    else {
      switch (fmt[++i]) {
        case 'd': {
          int iarg = va_arg(ap, int);
          snprintf (buf, sizeof(buf), "%d", iarg); 
          concat(s, buf);
          memset(buf, 0, 20);
          break;
        }
        case 's': {
          char* arg = va_arg(ap, char*);
          concat(s, buf);
          break;
        }
        default: append(s, fmt[i]);
      }
    }
  }
  return s;
}

string* str_with_len(u4 len) {
  string* s = new_empty_str();
  if (s->cap > len) 
    s->buf = GC_REALLOC(s->buf, len);
  return s;
}

void append(string* s, char c) {
  if (s->len == s->cap) {
    s->buf = GC_REALLOC(s->buf, s->cap + 20);
    s->cap += 20;
  }
  s->buf[s->len] = c;
  s->buf[s->len + 1] = '\0';
  s->len++;
}

int equals(const string* lhs, const char* rhs) {
  if (!rhs)
    return 0;
  if (lhs->len == strlen(rhs)) {
    u4 a = 0;
    while (a < lhs->len) {
      if (lhs->buf[a] != rhs[a]) 
        return 0;
      a++;
    }
    return 1;
  }
  
  return 0;
}

void concat(string* dest, const char* src) {
  int len = strlen(src);
  if (dest->cap + 1 < dest->len + len) {
    dest->cap = dest->len + len + 20;
    dest->buf = GC_REALLOC(dest->buf, dest->cap);
  }
  strcat(dest->buf, src);
  dest->len += len;
  dest->buf[dest->len] = '\0';
}

string* substr(string* s, const u4 start, const u4 end) {
  if (start >= end) 
    err("Trying to extract sub string when start index(%d) is more than end index(%d)", start, end);
  string* str = GC_MALLOC(sizeof(string));
  str->len = end - start;
  str->cap = str->len;
  str->buf = GC_MALLOC(sizeof(char) * str->cap);
  for (u4 i = start, j = 0; i < end; i++, j++) {
    str->buf[j] = s->buf[i];
  }
  return str;
}

char at(string *s, u4 index) {
  if (index == s->len) 
    err("Index %d is invalid for string of length %d", index, s->len);
  return s->buf[index];
}

int find(string* s, char c) {
  for (int i = 0; i < s->len; i++) {
    if (s->buf[i] == c) {
      return i;
    }
  }
  return -1;
}

int starts_with(const string* s, const char* str) {
  int len = strlen(str);
  if (len > s->len)
    return 0;
  for (int i = 0; i < len; i++) {
    if (s->buf[i] != str[i])
      return 0;
  }
  return 1;
}

int ends_with(const string* s, const char* str) {
  int len = strlen(str);
  if (len > s->len)
    return 0;
  for (int i = len - 1, j = s->len - 1; i > 0; i--, j--) {
    if (s->buf[j] != str[i]) {
      return 0;
    }
  }
  return 1;
}

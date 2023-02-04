#include <include/libcfreader.h>
#include <include/libutils.h>
#include <stdio.h>

#define CHECK(h) \
  if (h->pos == h->size) \
  err("Attempt to read file after content has been exhausted");

handle* open_file(char* name) {
  string* temp = new_str(name);
  if(find(temp, '.') == -1) {
    concat(temp, ".class");
  }
  FILE* f = fopen(temp->buf, "rb");
  if (f == NULL) 
    err("Could not read file %s", temp->buf);
  fseek(f, 0, SEEK_END);
  u8 size = ftell(f);
  fseek(f, 0, SEEK_SET);
  handle* h = GC_MALLOC(sizeof(handle));
  h->data = GC_MALLOC(sizeof(u1) * size);
  fread(h->data, sizeof(u1), size, f);
  h->size = size;
  h->pos = 0;
  fclose(f);
  return h;
}

inline u1 get_u1(handle* h) {
  CHECK(h);
  u1 read1 = h->data[h->pos];
  h->pos++;
  return read1;
}

inline u2 get_u2(handle* h) {
  u1 read1 = get_u1(h);
  u1 read2 = get_u1(h);
  return (read1 << 8) | read2;
}

u4 get_u4(handle* h) {
  u1 read1 = get_u1(h);
  u1 read2 = get_u1(h);
  u1 read3 = get_u1(h);
  u1 read4 = get_u1(h);
  return (read4 << 0) | (read3 << 8) | (read2 << 16) | (read1 << 24);
}

inline void skip(handle* h, u4 amt) {
  if (h->pos + amt >= h->size) 
    err("Cannot skip %s bytes as position exceeds file size", amt);
  h->pos += amt;
}

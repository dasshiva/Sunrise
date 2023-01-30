#include <include/libcfreader.h>
#include <include/libutils.h>
#include <stdlib.h>

#define CHECK(h) \
  if (h->pos == h->size) \
  err("Attempt to read file after content has been exhausted");

handle* open_file(char* name) {
  FILE* f = fopen(name, "rb");
  if (f == NULL) 
    err("Could not read file %s", name);
  fseek(f, 0, SEEK_END);
  u8 size = ftell(f);
  fseek(f, 0, SEEK_SET);
  handle* h = malloc(sizeof(handle));
  h->data = malloc(sizeof(u1) * size);
  fread(h->data, sizeof(u1), size, f);
  h->size = size;
  h->pos = 0;
  fclose(f);
  return h;
}

u1 get_u1(handle* h) {
  CHECK(h);
  u1 read1 = h->data[h->pos];
  h->pos++;
  return read1;
}

u2 get_u2(handle* h) {
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
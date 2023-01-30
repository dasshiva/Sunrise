#include <include/libcfreader.h>
#include <stdlib.h>

cpool* new_cpool(handle* h, u2 len){
  cpool* pool = malloc(sizeof(cpool));
  pool->elems = new_list();
  for (u2 i = 0; i < len; i++) {
    u1 tag = get_u1(h);
    switch (tag) {
      case UTF8: {
        utf8_elem* e = malloc(sizeof(utf8_elem));
      }
    }
  }
  return pool;
}
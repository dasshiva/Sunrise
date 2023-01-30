#include <include/libcfreader.h>
#include <stdlib.h>

cpool* new_cpool(handle* h, u2 len) {
  cpool* p = malloc(sizeof(cpool));
  p->elems = new_list();
  for (u2 i = 0; i < len; i++) {
    u1 tag = get_u1(h);
    pool_elem* pelem = malloc(sizeof(pool_elem));
    pelem->tag = tag;
    switch (tag) {
      case UTF8: {
        utf8_elem* e = malloc(sizeof(utf8_elem));
        e->length = get_u2(h);
        e->data = str_with_len(e->length);
        for (u2 i = 0; i < e->length; i++) {
          append(e->data, (char) get_u1(h));
        }
        pelem->elem.utf = e;
        break;
      }
      case MREF: {
        
      }
      default: err("Unknown tag: %d", tag);
    }
    add(p->elems, pelem);
  }
  return p;
}
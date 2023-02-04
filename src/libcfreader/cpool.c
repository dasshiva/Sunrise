#include <include/libcfreader.h>

list* new_cpool(handle* h, u2 len) {
  list* p = new_list();
  for (u2 i = 0; i < len-1; i++) {
    u1 tag = get_u1(h);
    pool_elem* pelem = GC_MALLOC(sizeof(pool_elem));
    pelem->tag = tag;
    dbg("Index %d has tag %d", i, tag);
    switch (tag) {
      case UTF8: {
        utf8_elem* e = GC_MALLOC(sizeof(utf8_elem));
        e->length = get_u2(h);
        e->data = str_with_len(e->length);
        for (u2 i = 0; i < e->length; i++) {
          append(e->data, (char) get_u1(h));
        }
        dbg("UTF8 content - %s", e->data->buf);
        pelem->elem.utf = e;
        break;
      }
      case INTEGER: {
        u4 bytes = get_u4(h);
        pelem->elem.integer = (i4) bytes;
        dbg("Integer content - %d", (i4) bytes);
        break;
      }
      case CLASS: {
        pelem->elem.class = get_u2(h);
        break;
      }
      case MREF: {
        mfiref_elem* melem = GC_MALLOC(sizeof(mfiref_elem));
        melem->class = get_u2(h);
        melem->nt = get_u2(h);
        pelem->elem.mref = melem;
        break;
      }
      case NTYPE: {
        ntype_elem* nelem = GC_MALLOC(sizeof(ntype_elem));
        nelem->name = get_u2(h);
        nelem->desc = get_u2(h);
        pelem->elem.nt = nelem;
        break;
      }
      default: err("Unknown tag: %d", tag);
    }
    add(p, pelem);
  }
  return p;
}

string* get_utf8(list* pool, u2 index) {
  pool_elem* p = get(pool, index - 1);
  switch (p->tag) {
    case UTF8: return p->elem.utf->data; 
    case CLASS: {
      u2 class = p->elem.class;
      return get_utf8(pool, class);
    }
    default: err("Index %d is not a CONSTANT_Utf8", index - 1);
  }
}

pool_elem* get_elem(list* pool, u2 index) {
    return get(pool, index - 1);
}
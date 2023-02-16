#include <include/libcfreader.h>
#include <math.h>

#ifndef NAN 
#error NAN is needed
#endif
#ifndef INFINITY
#error INFINITY is needed
#endif

list* new_cpool(handle* h, u2 len) {
  u1 add_empty = 0;
  list* p = new_list();
  for (u2 i = 0; i < len-1; i++) {
    u1 tag = get_u1(h);
    pool_elem* pelem = GC_MALLOC(sizeof(pool_elem));
    pelem->tag = tag;
    dbg("Index %d has tag %d", i, tag);
    switch (tag) {
      case UTF8: {
        u2 len = get_u2(h);
        string* e = str_with_len(len);
        for (u2 i = 0; i < len; i++) {
          append(e, (char) get_u1(h));
        }
        dbg("UTF8 content - %s", e->buf);
        pelem->elem.utf = e;
        break;
      }
      case INTEGER: {
        u4 bytes = get_u4(h);
        pelem->elem.integer = (i4) bytes;
        dbg("Integer content - %d", (i4) bytes);
        break;
      }
      case FLT: {
        u4 bytes = get_u4(h);
        if (bytes == 0x7f800000) 
          pelem->elem.flt = INFINITY;
        else if (bytes == 0xff800000) 
          pelem->elem.flt = -INFINITY;
        else if ((0x7f800001 <= bytes && bytes >= 0x7fffffff) || (0xff800001 <= bytes && bytes >= 0xffffffff))
          pelem->elem.flt = NAN;
        else {
          i4 bits = (i4) bytes;
          i4 s = ((bits >> 31) == 0) ? 1 : -1;
          i4 e = ((bits >> 23) & 0xff);
          i4 m = (e == 0) ? (bits & 0x7fffff) << 1 : (bits & 0x7fffff) | 0x800000;
          pelem->elem.flt = s * m * pow(2, e - 150);
        }
        dbg("Float content %f", pelem->elem.flt);
        break;
      }
      case LNG: {
        u4 high = get_u4(h);
        u4 low = get_u4(h);
        pelem->elem.lng = ((i8)high << 32) + low;
        dbg("Long content %li", pelem->elem.lng);
        add_empty = 1;
        i++;
        break;
      }
      case DBL: {
        u4 high = get_u4(h);
        u4 low = get_u4(h);
        i8 bits = ((i8)high << 32) + low;
        if (bits == 0x7ff0000000000000L)
          pelem->elem.dbl = INFINITY;
        else if (bits == 0xfff0000000000000L)
          pelem->elem.dbl = -INFINITY;
        else if ((0x7ff0000000000001L <= bits && bits >= 0x7fffffffffffffffL) || (0xfff0000000000001L <= bits && bits >= 0xffffffffffffffffL))
          pelem->elem.dbl = NAN;
        else {
          i4 s = ((bits >> 63) == 0) ? 1 : -1;
          i4 e = (int)((bits >> 52) & 0x7ffL); 
          i8 m = (e == 0) ? (bits & 0xfffffffffffffL) << 1 : (bits  & 0xfffffffffffffL) | 0x10000000000000L;
          pelem->elem.dbl = s * m * pow(2, e - 1075);
          dbg("Double content %lf", pelem->elem.dbl);
        }
        add_empty = 1;
        i++;
        break;
      }
      case CLASS: {
        pelem->elem.class = get_u2(h);
        break;
      }
      case STRING: {
        pelem->elem.string = get_u2(h);
        break;
      }
      case FIELD: {
        mfiref_elem* felem = GC_MALLOC(sizeof(mfiref_elem));
        felem->class = get_u2(h);
        felem->nt = get_u2(h);
        pelem->elem.fref = felem;
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
      case MHANDLE: {
        mhandle_elem* handle = GC_MALLOC(sizeof(mhandle_elem));
        handle->ref_kind = get_u1(h);
        handle->ref_index = get_u2(h);
        if (handle->ref_kind > 9 || handle->ref_kind < 1)
          err("Illegal method handle value %d", handle->ref_kind);
        pelem->elem.handle = handle;
        break;
      }
      case INVDYN: {
        invdyn_elem* in = GC_MALLOC(sizeof(invdyn_elem));
        in->bmeth = get_u2(h);
        in->nt = get_u2(h);
        pelem->elem.inv = in;
        break;
      }
      default: err("Unknown tag: %d", tag);
    }
    add(p, pelem);
    if (add_empty) {
      pool_elem* pe = GC_MALLOC(sizeof(pool_elem));
      pe->tag = EMP;
      add(p, pe);
      add_empty = 0;
    }
  }
  return p;
}

string* get_utf8(list* pool, u2 index) {
  pool_elem* p = get(pool, index - 1);
  switch (p->tag) {
    case UTF8: return p->elem.utf; 
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
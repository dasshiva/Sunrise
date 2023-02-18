#include <include/libutils.h>

list* new_list(void) {
  list* l = GC_MALLOC(sizeof(list));
  l->len = 0;
  l->index = 0;
  l->data = NULL;
  l->prev = NULL;
  l->next = NULL;
  return l;
}

static list* get_last(const list* l) {
  list* n = l;
  for(;;) {
    if (is_null(n->next)) {
      return n;
    }
    n = n->next;
  }
}
 
static void get_next(list* l) {
  l->next = GC_MALLOC(sizeof(list));
  l->next->index = l->index + 1;
  l->next->prev = l;
  l->next->data = NULL;
  l->next->next = NULL;
}

void add(list* l, void* data) {
  if (is_null(l->data)) {
    l->len++;
    l->data = data;
    return;
  }
  list* last = get_last(l);
  get_next(last);
  last->next->data = data;
  l->len++;
}

void* get(list* l, u4 index) {
  if (l->len <= index) {
    err("Index is %d but len is %d", index, l->len);
  }
  list* n = l;
  for(u4 i = 0; i < l->len; i++) {
    if (n->index == index) {
      return n->data;
    }
    n = n->next;
  }
}

void delete(list* l, u4 index) {
  if (l->len <= index) {
    return;
  }
  list* n = l;
  for(u4 i = 0; i < l->len; i++) {
    if (n->index == index) {
      if (n->prev != NULL)
        n->prev->next = n->next;
      if (n->next != NULL)
        n->next->prev = n->prev;
      n->data = NULL;
      l->len--;
      return;
   }
    n = n->next;
  }
}

void set(list* l, u4 index, void* data) {
   if (l->len <= index) 
    err("Accessing index %d when len is %d", index, l->len);
   list* n = l;
   for(u4 i = 0; i < l->len; i++) {
     if (n->index == index) {
       n->data = data;
       return;
     }
     n = n->next;
   }
}
#include <include/libutils.h>
#include <stdlib.h>

list* new_list(void) {
  list* l = malloc(sizeof(list));
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
  l->next = malloc(sizeof(list));
  l->next->index = l->index + 1;
  l->next->prev = l->next;
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

void* at(list* l, u4 index) {
  if (l->len <= index) {
    return NULL;
  }
  list* n = l;
  for(u4 i = 0; i <= index; i++) {
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
  for(u4 i = 0; i < index; i++) {
    if (n->index == i) {
      n->prev->next = n->next;
      n->next->prev = n->prev;
      free(n->data);
      free(n);
      return;
    }
    n = n->next;
  }
}

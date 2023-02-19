#include <include/libutils.h>

list* new_list(void) {
  list* ret = GC_MALLOC(sizeof(list));
  ret->len = 0;
  ret->head = NULL;
  return ret;
}

void add(list* l, void* data) {
  node* new = l->head;
  //dbg("%d", l->head == NULL);
  if (!new) {
    l->head = GC_MALLOC(sizeof(node));
    l->head->index = 0;
    l->head->data = data;
    l->len++;
    return;
  }
  while(new->next) {
    new = new->next;
  }
  new->next = GC_MALLOC(sizeof(node));
  new->next->index = new->index + 1;
  new->next->data = data;
  new->next->next = NULL;
  //dbg("%d", l->len);
  l->len++;
}

void* get(list* l, u4 index) {
  if (index >= l->len)
    err("Accessing index %d when length is %d", index, l->len);
  node* ret = l->head;
  while(ret->index != index) {
    ret = ret->next;
  }
  return ret->data;
}

void set(list* l, u4 index, void* data) {
  if (index >= l->len)
    err("Trying to set index %d when length is %d", index, l->len);
  node* ret = l->head;
  while(ret->index != index) {
    ret = ret->next;
  }
  ret->data = data;
}

void remove_last(list* l) {
  node* last = l->head;
  if (l->len == 0)
    err("Can't remove from empty list");
  else if (l->len == 1) 
    l->head = NULL;
  else {
    while (last && last->index < l->len - 2) {
      last = last->next;
    }
    last->next = NULL;
  }
  l->len--;
}
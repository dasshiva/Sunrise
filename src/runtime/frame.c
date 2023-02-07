#include <include/rt.h>
#include <stdlib.h>

frame* new_frame(method* m, list* cp) {
  if (is(m, NATIVE)) 
    return NULL;
  frame* f = GC_MALLOC(sizeof(frame));
  f->lvarray = new_list();
  attrs* at = get(m->attrs, 0);
  for (u2 i = 0; i < code(at).locals; i++) {
    elem* e = GC_MALLOC(sizeof(elem));
    e->t = EMPTY;
    add(f->lvarray, e);
  }
  f->stack = new_list();
  f->mt = m;
  f->cp = cp;
  return f;
}

void push(frame* f, elem* data) {
  attrs* c = get(f->mt->attrs, 0);
  if (f->stack->len == code(c).stack) 
    err("Stack overflow while executing method %s%s", f->mt->name->buf, f->mt->desc->buf);
  add(f->stack, data);
}

void* pop(frame* f) {
  if (f->stack->len == 0)
    err("Stack underflow while executing method %s%s", f->mt->name->buf, f->mt->desc->buf);
  void* el = get(f->stack, f->stack->len - 1);
  delete(f->stack, f->stack->len - 1);
  return el;
}
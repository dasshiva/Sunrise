#include <include/rt.h>
#include <stdlib.h>

static void parse_desc(frame* f);

frame* new_frame(method* m, list* cp, string* class) {
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
  f->class = class;
  f->stack = new_list();
  f->mt = m;
  f->cp = cp;
  parse_desc(f);
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

static void parse_desc(frame* f) {
  string* desc = f->mt->desc;
  u2 arg = 0;
  int end = find(desc, ')');
  if (desc->buf[0] != '(' || end == -1)
    err("Method signature is invalid");
  string* params = NULL;
  if (end != 1)
    params = substr(desc, 1, end);
  if (params) {
    for (u2 i = 0; i < params->len; i++) {
      switch(params->buf[i]) {
        case 'B':
        case 'C':
        case 'Z':
        case 'I': {
          elem* e = get(f->lvarray, arg++);
          e->t = INT;
          break;
        }
        case 'F': {
          elem* e = get(f->lvarray, arg++);
          e->t = FLOAT;
          break;
        }
        case 'J': {
          elem* e = get(f->lvarray, arg++);
          e->t = LONG;
          break;
        }
        case 'D': {
          elem* e = get(f->lvarray, arg++);
          e->t = DOUBLE;
          break;
        }
        case 'L': {
          int semi = find(params, ';');
          if (semi + 1 != end) {
            i = 0;
            params = substr(params, semi + 1, end);
          }
          else 
            i = semi;
          elem* e = get(f->lvarray, arg++);
          e->t = REF;
          break;
        }
        default: err("Arrays not implemented");
      }
    }
  }
  f->args = arg;
  switch(desc->buf[end + 1]) {
    case 'B': f->ret = BYTE; break;
    case 'C': f->ret = CHAR; break;
    case 'Z': f->ret = BOOL; break;
    case 'I': f->ret = INT; break;
    case 'F': f->ret = REF; break;
    case 'J': f->ret = LONG; break;
    case 'D': f->ret = DOUBLE; break;
    case 'L': f->ret = REF; break;
    case 'V': f->ret = EMPTY; break;
    default: err("Not implemented %c", desc->buf[end + 1]);
  }
}
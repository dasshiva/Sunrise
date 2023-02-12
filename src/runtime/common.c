#include <include/types.h>
#include <include/rt.h>

asm("globl _hashCode");
i4 hashCode(elem* obj) {
  return (int) obj->data.ref;
}
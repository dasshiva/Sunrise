#include <include/types.h>
#include <include/rt.h>

i4 hashCode(elem* obj) {
  return (int) obj->data.ref;
}
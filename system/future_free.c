#include <xinu.h>
#include <future.h>

syscall future_free(future *f) {
  return freemem(f, sizeof(future));
}
/*
Description
  Free the allocated future. This function will simply free the space assigned to Future. Use freemem(). call to free the space.

Parameters
  future* f: future to free

Return
  syscall: SYSERR or OK
 */

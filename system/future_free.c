#include <xinu.h>
#include <future.h>

syscall future_free(future* f) {
  freemem((*f).value, 4);
  freemem(f, 16);
  return OK;
}
/*
Description
  Free the allocated future. This function will simply free the space assigned to Future. Use freemem(). call to free the space.

Parameters
  future* f: future to free

Return
  syscall: SYSERR or OK
 */

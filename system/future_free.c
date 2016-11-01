#include <xinu.h>
#include <future.h>

syscall future_free(future *f) {
  int32 status;
  status = freemem(f->value, 4);
  if (status == SYSERR) {
    return SYSERR;
  }
  status = freemem(f->get_queue, sizeof(quentry)*(MAX_FUTURE_LENGTH+2));
  if (status == SYSERR) {
    return SYSERR;
  }
  status = freemem(f->set_queue, sizeof(quentry)*(MAX_FUTURE_LENGTH+2));
  if (status == SYSERR) {
    return SYSERR;
  }
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

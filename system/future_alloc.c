#include <xinu.h>
#include <future.h>

future *future_alloc(int future_flags) {
  future *pt = getmem(sizeof(future));
  if ((int) pt == SYSERR) {
    printf("Get memory failed");
    return NULL;
  }

  (*pt).flag = future_flags;
  (*pt).state = FUTURE_EMPTY;
  (*pt).pid = 0;
  pt->value = getmem(4);

  printf("before return\n");
  return pt;
}

/*
Description
Allocates a new future (in the FUTURE_EMPTY state) in given mode. We will use getmem() call to allocate space for the new future.

Parameters
int future_flag: flag to be set for this future

Return
future: NULL or a valid future reference

*/

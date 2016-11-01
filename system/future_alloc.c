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

  pt->get_queue = getmem(sizeof(quentry)*(MAX_FUTURE_LENGTH+2));
  if ((int) pt->get_queue == SYSERR) {
    printf("Get memory failed");
    return NULL;
  }
  pt->get_queue[0].qprev = NULL;
  pt->get_queue[0].qnext = 1;
  pt->get_queue[1].qprev = 0;
  pt->get_queue[1].qnext = NULL;

  pt->set_queue = getmem(sizeof(quentry)*(MAX_FUTURE_LENGTH+2));
  if ((int) pt->set_queue == SYSERR) {
    printf("Get memory failed");
    return NULL;
  }
  pt->set_queue[0].qprev = NULL;
  pt->set_queue[0].qnext = 1;
  pt->set_queue[1].qprev = 0;
  pt->set_queue[1].qnext = NULL;

  for (int32 i = 0; i < (MAX_FUTURE_LENGTH+2); i++) {
    pt->get_queue[i].processid = 0;
    pt->set_queue[i].processid = 0;
  }

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

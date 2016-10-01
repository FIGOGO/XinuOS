#include <xinu.h>
#include <future.h>

syscall future_set(future* f, int* j) {
  int currentpid = getpid();
  while ((*f).state == FUTURE_VALID) {
    (*f).pid = currentpid;
    suspend(currentpid);
  }

  *(*f).value = *j;
  (*f).state = FUTURE_VALID;

  if ((*f).pid > 0 &&  (*f).pid != currentpid){
    resume((*f).pid);
  }

  return OK;

}

  /*
  if ((*f).state == FUTURE_EMPTY || (*f).state == FUTURE_WAITING) {
    (*f).value = j;
    (*f).state = FUTURE_VALID;
    return OK;
  }
  else return SYSERR;
}
/*
Description
  Set a value in a future and changes state from FUTURE_EMPTY or FUTURE_WAITING to FUTURE_VALID.

Parameters
  future *f: future in which to set the value int *value: result of an operation to be set as value in a future

Return
  syscall: SYSERR or OK

 */

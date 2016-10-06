#include <xinu.h>
#include <future.h>

syscall future_get(future *f, int *i) {
  int currentpid = getpid();

  if ((*f).state < FUTURE_VALID) {
    (*f).pid = currentpid;
    int status = suspend(currentpid);
    if (status == SYSERR){
      printf("Process suspend failed in future get");
      return SYSERR;
    }
  }

  *i = *(*f).value;
  (*f).state = FUTURE_WAITING;

  if ((*f).pid > 0 && (*f).pid != currentpid) {
    resume((*f).pid);
  }

  return OK;

}



  /*
  if ((*f).state == FUTURE_VALID) {
    i = (*f).value;
    (*f).state = FUTURE_WAITING;
    return OK;
  }
  else if((*f).state == FUTURE_WAITING || (*f).state == FUTURE_EMPTY){

    return SYSERR;
  }
}

/*
Description
  Get the value of a future set by an operation and change the state of future from FUTURE_VALID to FUTURE_EMPTY.

Parameters
  future *f: future on which to wait for and obtain value
  int *value: variable in which to obtain the value from a future

Return
  syscall: SYSERR or OK

 */

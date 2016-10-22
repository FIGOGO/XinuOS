#include <xinu.h>
#include <future.h>

syscall future_get(future *f, int *i) {
  intmask mask;
  mask = disable();
  int current_pid = getpid();


  if (f->flag == FUTURE_EXCLUSIVE) {
    if ((*f).state < FUTURE_VALID) {
      (*f).pid = current_pid;
      int status = suspend(current_pid);
      if (status == SYSERR){
        printf("Process suspend failed in future get");
        restore(mask);
        return SYSERR;
      }
    }

    *i = *(*f).value;
    (*f).state = FUTURE_WAITING;
    if (!isbadpid(f->pid) && (*f).pid != current_pid) {
      resume((*f).pid);
    }
    /* The following code assumes only one set and get.
    The above code is backup for multiply sets and gets procedure. No use in one set get case.
    The two parts of code are for different situations. But can NOT work together.*/
    future_free(f);
    restore(mask);
    return OK;
  }

  else if (f->flag == FUTURE_SHARED) {
    if (f->state < FUTURE_VALID) {
      future_enqueue(getpid(), f->get_queue);
      int status = suspend(getpid());
      if (status == SYSERR){
        printf("Process suspend failed in future get");
        restore(mask);
        return SYSERR;
      }
    }

    *i = *(f->value);
    if (!isbadpid(f->pid) && f->pid != current_pid) {
      resume(f->pid);
    }

    restore(mask);
    return OK;
  }

  else if (f->flag == FUTURE_QUEUE) {
    if (f->state < FUTURE_VALID) {
      future_enqueue(getpid(), f->get_queue);
      int status = suspend(getpid());
      if (status == SYSERR){
        printf("Process suspend failed in future get");
        restore(mask);
        return SYSERR;
      }
    }

    *i = *(f->value);
    f->state = FUTURE_WAITING;
    if (is_not_empty(f->set_queue)){
      resume(future_dequeue(f->set_queue));
    }
    restore(mask);
    return OK;
  }

  else {
    restore(mask);
    return SYSERR;
  }
}



 

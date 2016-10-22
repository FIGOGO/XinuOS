#include <xinu.h>
#include <future.h>

syscall future_set(future *f, int *j) {
  intmask mask;
  int current_pid = getpid();
  mask = disable();

  if (f->flag == FUTURE_EXCLUSIVE) {
    if ((*f).state == FUTURE_VALID) {
      (*f).pid = current_pid;
      int32 status = suspend(current_pid);
      if (status == SYSERR){
        printf("Process suspend failed in future set");
        restore(mask);
        return SYSERR;
      }
    }

    *(*f).value = *j;
    (*f).state = FUTURE_VALID;
    if ((*f).pid > 0 &&  (*f).pid != current_pid){
      resume((*f).pid);
    }
    restore(mask);
    return OK;
  }

  else if (f->flag == FUTURE_SHARED){
    if (f->state == FUTURE_VALID){
      int32 status = suspend(current_pid);
      if (status == SYSERR){
        printf("Process suspend failed in future set");
        restore(mask);
        return SYSERR;
      }
    }

    *(f->value) = *j;
    f->state = FUTURE_VALID;
    while (is_not_empty(f->get_queue)) {
      resume(future_dequeue(f->get_queue));
    }
    restore(mask);
    return OK;
  }

  else if (f->flag == FUTURE_QUEUE) {
    if (f->state == FUTURE_VALID){
      future_enqueue(current_pid, f->set_queue);
      int32 status = suspend(current_pid);
      if (status == SYSERR) {
        printf("Process suspend failed in future set");
        restore(mask);
        return SYSERR;
      }
    }

    *(f->value) = *j;
    f->state = FUTURE_VALID;
    if (is_not_empty(f->get_queue)) {
      resume(future_dequeue(f->get_queue));
    }
    restore(mask);
    return OK;

 }

  else {
    restore(mask);
    return SYSERR;
  }
}


#include <xinu.h>
#include <future.h>

pid32 future_enqueue(pid32 process_id, quentry *queue) {
  int32 origional = queue[1].qprev;
  int32 position = 0;
  for (int32 i = 2; i < (MAX_FUTURE_LENGTH+2); i++) {
    if (queue[i].processid == 0) {
      position = i;
      break;
    }
  }

  if(position == 0) {
    return SYSERR;
  }

  queue[origional].qnext = position;
  queue[1].qprev = position;
  queue[position].qprev = origional;
  queue[position].qnext = 1;
  queue[position].processid = process_id;
  return OK;
}


pid32 future_dequeue(quentry *queue) {
  if (!is_not_empty) {
    return SYSERR;
  }

  int32 next = queue[0].qnext;
  queue[0].qnext = queue[next].qnext;
  queue[queue[next].qnext].qprev = 0;
  pid32 process_id = queue[next].processid;
  queue[next].processid = 0;
  return process_id;
}

bool is_not_empty(quentry * queue){
  if (queue[0].qnext == 1) {
    return 0;
  }
  else return 1;
};


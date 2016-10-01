#include <xinu.h>
#include <future.h>
#include <prodcons.h>

uint future_prod(future *fut) {
  int i, j, status;
  j = (int)fut;
  for (i=0; i<1000; i++) {
    j += i;
  }

  status = future_set(fut, &j);
  if (status < 1){
    printf("future_set failed\n");
    return -1;
  }

  return OK;
}


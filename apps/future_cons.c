#include <xinu.h>
#include <future.h>
#include <prodcons.h>

uint future_cons(future *fut) {
  int i, status;
  status = future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("it produced %d\n", i);
  /* future_free(fut); */
  return OK;
}

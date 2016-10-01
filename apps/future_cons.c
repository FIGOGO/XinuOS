#include <xinu.h>
#include <future.h>
#include <prodcons.h>

uint future_cons(future *fut) {
  int i, status;
  printf("Before get, the i value is %d. At %d\n", i, &i);
  printf("Before get, the future value is %d. At %d\n",*(*fut).value, (*fut).value);
  status = future_get(fut, &i);
  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  //printf("it produced %d\n", i);
  printf("After get, the i value is %d. At %d\n", i, &i);
  printf("After get, the future value is %d. At %d\n", *(*fut).value, (*fut).value);
  return OK;
}

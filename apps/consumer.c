#include <xinu.h>
#include <prodcons.h>

void consumer(int32 count) {
  //Code to consume values of global variable 'n' until the value
  // of n is less than or equal to count
  //print consumed value e.g. consumed : 8
  for (int32 i=1; i<count; i++) {
    printf (n);
  }
}


#include <xinu.h>
#include <prodcons.h>

void consumer(int32 count, sid32 consumed, sid32 produced) {
  //Code to consume values of global variable 'n' until the value
  // of n is less than or equal to count
  //print consumed value e.g. consumed : 8
  for (int32 i=0; i<=count; i++) {
    wait (produced);
    printf ("comsumed: %d \n", n);
    signal (consumed);
  }
}


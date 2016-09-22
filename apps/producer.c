#include <xinu.h>
#include <prodcons.h>

void producer(int32 count) {
  //Code to produce values less than equal to count,
  //produced value should get assigned to global variable 'n'.
  //print produced value e.g. produced : 8
  int32 i;
  for(i = 1; i<count; i++){
    n++;
  }
}

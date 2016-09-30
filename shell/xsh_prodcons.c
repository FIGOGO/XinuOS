#include <xinu.h>
#include <prodcons.h>

int32 n;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int32 nargs, char *args[])
{
  //Argument verifications and validations
  int32 count = 2000;
  n = 0;
  //local varible to hold count
  //check args[1] if present assign value to count

  //create the process producer and consumer and put them in ready queue.
  //Look at the definations of function create and resume in the system folder for reference.

  if (nargs > 2){
    printf ("too many arguments\n");
    return (-1);
  }
  if (nargs == 2){
    count = atoi(args[1]);
  }

  // semaphore set up
  sid32 produced = semcreate(0);
  sid32 consumed = semcreate(1);

  resume( create(consumer, 1024, 20, "consumer", 3, count, consumed, produced));
  resume( create(producer, 1024, 20, "producer", 3, count, consumed, produced));
  return (0);
}
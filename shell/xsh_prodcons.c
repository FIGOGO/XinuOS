#include <xinu.h>
#include <prodcons.h>

int32 n = 0;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int32 nargs, char *args[])
{
  //Argument verifications and validations
  int count = 2000;
  //local varible to hold count
  //check args[1] if present assign value to count

  //create the process producer and consumer and put them in ready queue.
  //Look at the definations of function create and resume in the system folder for reference.
  if (nargs > 2){
    printf ("too many arguments\n");
    return -1;
  }

  if (nargs == 2){
    count = (int32) args[1];
  
  
  resume( create(consumer, 1024, 20, "consumer", 1, 2000));
  resume( create(producer, 1024, 20, "producer", 1, 2000));
  return (0);}
}

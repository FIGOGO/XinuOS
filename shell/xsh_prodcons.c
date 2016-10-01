#include <xinu.h>
#include <future.h>
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

  future *f1, *f2, *f3;

  f1 = future_alloc(FUTURE_EXCLUSIVE);
  f2 = future_alloc(FUTURE_EXCLUSIVE);
  f3 = future_alloc(FUTURE_EXCLUSIVE);
  printf("Origional f value is %d. At %d\n", *(*f1).value,(*f1).value);

  resume( create(future_prod, 1024, 20, "fprod1", 1, f1) );
  resume( create(future_cons, 1024, 20, "fcons1", 1, f1) );
  /*
  resume( create(future_cons, 1024, 20, "fcons2", 1, f2) );
  resume( create(future_prod, 1024, 20, "fprod2", 1, f2) );
  resume( create(future_cons, 1024, 20, "fcons3", 1, f3) );
  resume( create(future_prod, 1024, 20, "fprod3", 1, f3) );
  */
  return (0);
}

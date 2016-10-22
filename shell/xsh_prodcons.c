#include <xinu.h>
#include <future.h>
#include <prodcons.h>

int32 n;                 //Definition for global variable 'n'
/*Now global variable n will be on Heap so it is accessible all the processes i.e. consume and produce*/

shellcmd xsh_prodcons(int32 nargs, char *args[])
{
  //Argument verifications and validations
  n = 0;
  //local varible to hold count
  //check args[1] if present assign value to count
  // if possible atoi(args[1]);

  if (nargs > 2){
    printf ("too many arguments\n");
    return (-1);
  }

  if (nargs == 2){
    if (strncmp(args[1], "--help", 7) == 0) {
      printf("This command is to coordinate the producer and consumer.\n");
      printf("If the argument is -f, the command will use future to coordinate the producer and consumer\n");
    }

    else if (strncmp(args[1], "-f", 3) == 0) {

      /* future *f1, *f2, *f3; */
      /* f1 = future_alloc(FUTURE_EXCLUSIVE); */
      /* f2 = future_alloc(FUTURE_EXCLUSIVE); */
      /* f3 = future_alloc(FUTURE_EXCLUSIVE); */
      /* resume( create(future_cons, 1024, 20, "fcons1", 1, f1) ); */
      /* resume( create(future_prod, 1024, 20, "fprod1", 1, f1) ); */
      /* resume( create(future_cons, 1024, 20, "fcons2", 1, f2) ); */
      /* resume( create(future_prod, 1024, 20, "fprod2", 1, f2) ); */
      /* resume( create(future_cons, 1024, 20, "fcons3", 1, f3) ); */
      /* resume( create(future_prod, 1024, 20, "fprod3", 1, f3) ); */

      future *f_exclusive, *f_shared, *f_queue;

      f_exclusive = future_alloc(FUTURE_EXCLUSIVE);
      f_shared = future_alloc(FUTURE_SHARED);
      f_queue = future_alloc(FUTURE_QUEUE);

      // Test FUTURE_EXCLUSIVE
      resume( create(future_cons, 1024, 20, "fcons1", 1, f_exclusive) );
      resume( create(future_prod, 1024, 20, "fprod1", 1, f_exclusive) );

      // Test FUTURE_SHARED
      resume( create(future_cons, 1024, 20, "fcons2", 1, f_shared) );
      resume( create(future_cons, 1024, 20, "fcons3", 1, f_shared) );
      resume( create(future_cons, 1024, 20, "fcons4", 1, f_shared) );
      resume( create(future_cons, 1024, 20, "fcons5", 1, f_shared) );
      resume( create(future_prod, 1024, 20, "fprod2", 1, f_shared) );

      // Test FUTURE_QUEUE
      resume( create(future_cons, 1024, 20, "fcons6", 1, f_queue) );
      resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
      resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
      resume( create(future_cons, 1024, 20, "fcons7", 1, f_queue) );
      resume( create(future_prod, 1024, 20, "fprod3", 1, f_queue) );
      resume( create(future_prod, 1024, 20, "fprod4", 1, f_queue) );
        resume( create(future_prod, 1024, 20, "fprod5", 1, f_queue) );
      resume( create(future_prod, 1024, 20, "fprod6", 1, f_queue) );

    }

  }

 return (0);
}

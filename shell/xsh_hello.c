#include <xinu.h>
#include <stdio.h>

shellcmd xsh_hello (int nargs, char *args[]) {
  if (nargs < 2) {
    printf("error: too few arguments\n");}
  else if (nargs >2) {
    printf("error: too many arguments\n");}
  else {
    printf("Hello %s. Welcome to the world of Xinu!!\n", args[1]);}
  return 0;
}	

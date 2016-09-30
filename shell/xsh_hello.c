#include <xinu.h>
#include <stdio.h>

shellcmd xsh_hello (int32 nargs, char *args[]) {
  if (nargs < 2) {    
	printf("error: too few arguments\n");
	return 1;}
  else if (nargs >2) {
	printf("error: too many arguments\n");
	return 2;}
  else {
	printf("Hello %s. Welcome to the world of Xinu!!\n", args[1]);
	return 0;}
}	

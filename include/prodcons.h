/*Global variable for producer consumer*/
extern int32 n; /*this is just declaration*/

/*function Prototype*/
void consumer(int32 count, sid32 consumed, sid32 produced);
void producer(int32 count, sid32 consumed, sid32 produced);


/* Semaphore Declarations */
extern sid32 produced, consumed; 

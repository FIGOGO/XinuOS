#ifndef _FUTURE_H_
#define _FUTURE_H_

/* define states */
#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1
#define FUTURE_VALID 	  2

/* modes of operation for future*/
#define FUTURE_EXCLUSIVE  1
#define FUTURE_SHARED     2
#define FUTURE_QUEUE      3

typedef struct queueentry
{
  pid32 processid;
  int32 qnext;
  int32 qprev;
} quentry;

typedef struct futent
{
  int32 *value;
  int32 flag;
  int32 state;
  pid32 pid;
  quentry *set_queue;
  quentry *get_queue;
} future;

#define MAX_FUTURE_LENGTH 10

pid32 future_eequeue(pid32 process_id, quentry *queue);
pid32 future_dnqueue(quentry *queue);
bool is_not_empty(quentry * queue);

/* Interface for system call */
future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);

uint future_prod(future*);
uint future_cons(future*);

#endif /* _FUTURE_H_ */

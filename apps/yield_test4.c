#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <assert.h>

int tid1, tid2;

// child thread joins finished parent
int thread2()
{
  int ret;
  printf("thread2\n");
  uthread_join(tid1, &ret);
  printf("thread1 returned %d\n", ret);

  return 2;
}

int thread1()
{
  tid2 = uthread_create(thread2);
  printf("thread1\n");

  return 1;
}

int main()
{
  fprintf(stderr, "\n*** test child joins parent ***\n");
  int ret;

  uthread_start(0);

  tid1 = uthread_create(thread1);
  uthread_yield();

  uthread_join(tid2, &ret);
  printf("thread2 returned %d\n", ret);
  uthread_stop();

  return 0; 
}
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <assert.h>
#include <queue.h>

// thread 1 joins thread2, thread 3 joins thread 1
// (thread 1 is blocked by thread 2)
int thread3(void)
{ 
  int ret;

  printf("thread%d\n", uthread_self());
  uthread_join(1, &ret);

  printf("thread 1 returned %d\n", ret);
  return 3;
}

int thread2(void)
{
  printf("thread%d\n", uthread_self());
  uthread_create(thread3);
  uthread_yield();
  return 2;
}

int thread1(void)
{
  int ret;

  printf("thread%d\n", uthread_self());
  uthread_join(uthread_create(thread2), &ret);

  printf("thread 2 returned %d\n", ret);
  return 1;
}

int main(void)
{
  fprintf(stderr, "\n*** test join blocked thread ***\n");
  int ret;

  uthread_start(0);

  uthread_create(thread1);
  uthread_yield();
  uthread_yield();
  uthread_join(3, &ret);

  printf("thread 3 returned %d\n", ret);
  assert(uthread_stop() == 0);

  return 0;
}

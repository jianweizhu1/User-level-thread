#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <assert.h>

// error checking: call self, main, invalid tid
int thread4(void)
{
  printf("thread%d\n", uthread_self());
  assert(uthread_join(0, NULL) == -1);
  assert(uthread_join(uthread_self(), NULL) == -1);
  assert(uthread_join(8, NULL) == -1);
  return 0;
}

// test join functionality
int thread3(void)
{ 
  printf("thread%d\n", uthread_self());
  return 0;
}

int thread2(void)
{
  printf("thread%d\n", uthread_self());
  uthread_join(uthread_create(thread3), NULL);
  return 0;
}

int thread1(void)
{
  printf("thread%d\n", uthread_self());
  uthread_join(uthread_create(thread2), NULL);
  return 0;
}

int main(void)
{
  uthread_start(0);

  fprintf(stderr, "\n*** test join functionality ***\n");
  uthread_join(uthread_create(thread1), NULL);

  fprintf(stderr, "\n*** error checking for invalid tid ***\n");
  int tid4 = uthread_create(thread4);
  uthread_yield();
  uthread_join(tid4, NULL);

  assert(uthread_stop() == 0);

  return 0;
}

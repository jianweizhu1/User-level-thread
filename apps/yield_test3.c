#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <assert.h>

int tid1, tid2, tid3, tid4;

// try to join the same thread tid4 (already collected)
int thread4(void)
{
  printf("thread%d\n", uthread_self());
  return 0;
}

int thread3(void)
{
  printf("thread%d\n", uthread_self());
  uthread_join(tid4, NULL);
  return 0;
}

// try to join the same thread tid2 which is still in ready queue
int thread2(void)
{
  printf("thread%d\n", uthread_self());
  return 0;
}

int thread1(void)
{
  printf("thread%d\n", uthread_self());
  assert(uthread_join(tid2, NULL) == -1);
  return 0;
}

int main(void)
{
  uthread_start(0);

  fprintf(stderr, "\n*** test 1 join same thread ***\n");
  tid1 = uthread_create(thread1);
  tid2 = uthread_create(thread2);
  uthread_join(tid2, NULL);
  uthread_join(tid1, NULL);

  fprintf(stderr, "\n*** test 2 join same thread ***\n");

  tid3 = uthread_create(thread3);
  tid4 = uthread_create(thread4);
  uthread_join(tid3, NULL);
  assert(uthread_join(tid4, NULL) == -1);

  assert(uthread_stop() == 0);

  return 0;
}

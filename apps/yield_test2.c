#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <assert.h>

// join returns immediately after collecting a finished thread
int thread1(void)
{
  printf("thread%d\n", uthread_self());
  return 6;
}

int main(void)
{
  uthread_start(0);

  fprintf(stderr, "\n*** test collect finished thread ***\n");

  int tid1 = uthread_create(thread1);
  uthread_yield();

  printf("thread 1 in finished queue\n");

  int status;
  uthread_join(tid1, &status);

  printf("t1 exited with status %d\n", status);
  assert(status == 6);

  assert(uthread_stop() == 0);

  return 0;
}

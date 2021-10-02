#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>
#include <assert.h>
#include <time.h>

#define FIB 40

int fib[FIB+1];

int done = 0;

int slow_fib(int n) {
  if (n < 2)
    return n;

  return slow_fib(n-2) + slow_fib(n-1);
}

// test preemption by calculating first 40 fibonacci numbers recursively
int thread2()
{
  fprintf(stderr, "\n*** test fibonacci calculation ***\n");
  int num = 0;

  for(int i = 0; i <= FIB; i++) {
    num = slow_fib(i);
    printf("%d: %d\n", i, num);
    assert(num == fib[i]);
  }

  done = 1;

  return 0;
}

// test preemption using elapsed time
int thread1(void)
{
  fprintf(stderr, "\n*** test 1 second preemption ***\n");
  printf("\ntimer set\n");

  clock_t start = clock();
  double elapsed_time = 0;

  while(elapsed_time < 1)
    elapsed_time = ((double) (clock() - start)) / CLOCKS_PER_SEC;

  printf("%.2f seconds elapsed\n\n", elapsed_time);

  return 0;
}

int main(void)
{
  // generate fibonacci sequence before multithreading
  fib[0] = 0;
  fib[1] = 1;

  for(int i = 2; i <= FIB; i++)
    fib[i] = fib[i-1] + fib[i-2];

  // first thread: test 1 second preemption
  uthread_start(1);
  int tid1 = uthread_create(thread1);
  uthread_yield();

  printf("thread 1 yielded to main\n");
  uthread_yield();
  printf("thread 1 yielded to main again\n");
  uthread_join(tid1, NULL);

  // second thread: test fibonacci calculation
  int tid2 = uthread_create(thread2);
  uthread_yield();

  //printf("thread 2 yielded to main\n");

  while(done == 0) {
    printf("thread 2 yielded to main\n");
    uthread_yield();
  }

  uthread_join(tid2, NULL);

  printf("\nuthread library exited with status %d\n", uthread_stop());
  
  return 0;
}

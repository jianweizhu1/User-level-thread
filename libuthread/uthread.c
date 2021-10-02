#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

typedef struct TCB {
  uthread_t tid;
  int exit_status;
  uthread_ctx_t ctx;
  void* stack;
  struct TCB* blocked_thread;
} TCB;  

// tid number
uthread_t tid_count = 0;

// queue of ready threads (represented by TCB)
queue_t scheduled_q;

// queue of blocked threads
queue_t blocked_q;

// queue of finished threads (zombie threads)
queue_t finished_q;

// pointer to active thread
TCB* active_thread;

// enable preemption
int enable_interrupts = 0;

// returns the oldest node in the queue with a certain value
static int find_tcb(queue_t q, void* data, void* arg)
{
  TCB* cur_thread = (TCB*)data;
  uthread_t* tid = (uthread_t*)arg;
  
  (void)q; //unused

  if (cur_thread->tid == *tid)
    return 1;

  return 0;
}

// initialize objects and register main thread
int uthread_start(int preempt)
{
  // library already started
  if(active_thread)
    return -1;

  enable_interrupts = preempt;

  if(preempt)
    preempt_start();

  scheduled_q = queue_create();
  blocked_q = queue_create();
  finished_q = queue_create();

  TCB* main_thread = malloc(sizeof(TCB));

  // memory allocation error
  if(!main_thread || !scheduled_q || !blocked_q || !finished_q)    
    return -1;

  main_thread->tid = 0;
  main_thread->blocked_thread = NULL;

  // set active thread to main thread
  active_thread = main_thread;
  
  return 0;
}

// frees resources used by thread library
// all threads must be joined for library to be stopped
int uthread_stop(void)
{
  // called from another thread besides main
  if(!active_thread || active_thread->tid != 0)
    return -1;

  // return -1 if any of the queues are nonempty
  if(queue_length(scheduled_q) || queue_length(finished_q) ||
    queue_length(blocked_q))
    return -1;

  // free main thread tcb
  free(active_thread);        

  assert(queue_destroy(finished_q) == 0);
  assert(queue_destroy(blocked_q) == 0);
  assert(queue_destroy(scheduled_q) == 0);

  if(enable_interrupts)
    preempt_stop();

  return 0;
}

// makes a new user thread
int uthread_create(uthread_func_t func)
{
  // tid value overflow
  if (tid_count == USHRT_MAX)
    return -1;

  if (!func)
    return -1;

  TCB* t_new = malloc(sizeof(TCB));

  if(!t_new)       // malloc error
    return -1;

  t_new->stack = uthread_ctx_alloc_stack();
  t_new->blocked_thread = NULL;

  // start of critical section
  preempt_disable();

  // initialize execution context for uthread
  if (uthread_ctx_init(&t_new->ctx, t_new->stack, func)) {
    uthread_ctx_destroy_stack(t_new->stack);
    return -1;
  }

  tid_count++;
  t_new->tid = tid_count;

  queue_enqueue(scheduled_q, (void*)t_new);

  // end of critical section
  preempt_enable();

  return tid_count;
}

// context switch from active thread to next scheduled thread in queue
void uthread_yield(void)
{ 
  TCB* prev_thread = active_thread;

  preempt_disable();

  // no other ready threads
  if(queue_length(scheduled_q) == 0)         
    return;

  // add currently running thread to queue
  // set active thread to next scheduled thread
  queue_enqueue(scheduled_q, active_thread);
  queue_dequeue(scheduled_q, (void**)&active_thread);

  // context switch to next thread
  uthread_ctx_switch(&prev_thread->ctx, &active_thread->ctx);

  // enable interrupts before returning to user thread
  preempt_enable();
}

uthread_t uthread_self(void)
{
  return active_thread->tid;
}

// set exit status and add thread to finished queue 
void uthread_exit(int retval)
{
  TCB* finished_thread = active_thread;
  finished_thread->exit_status = retval;

  TCB* blocked_thread = finished_thread->blocked_thread;

  // start of critical section
  preempt_disable();

  // unblock the blocked thread (if any) when current thread exits
  if (blocked_thread != NULL) {
    queue_delete(blocked_q, blocked_thread);
    queue_enqueue(scheduled_q, blocked_thread);
  }

  // add finished thread to finished queue
  assert(queue_enqueue(finished_q, (void*)finished_thread) == 0);

  // context switch to next thread
  assert(queue_dequeue(scheduled_q, (void**)&active_thread) == 0);
  uthread_ctx_switch(&finished_thread->ctx, &active_thread->ctx);

  // we will never return from context switch
}

// block current thread until we collect exit status of target thread
int uthread_join(uthread_t tid, int* retval)
{
  // cannot join self or main thread
  if (tid == 0 || tid == active_thread->tid)
    return -1;

  // search for tid in ready queue or blocked queue
  TCB* target_t = NULL;

  // start of critical section
  preempt_disable();
  queue_iterate(scheduled_q, find_tcb, &tid, (void**)&target_t);

  if(target_t == NULL)
    queue_iterate(blocked_q, find_tcb, &tid, (void**)&target_t);
  
  // end of critical section
  preempt_enable();

  // if target thread was found, set its blocked field
  // context switch to next scheduled thread
  if (target_t != NULL) {
    // only one thread can join target 
    if(target_t->blocked_thread != NULL)    
      return -1;

    TCB* prev_thread = active_thread;
    target_t->blocked_thread = prev_thread;

    // start of critical section
    preempt_disable();
    assert(queue_enqueue(blocked_q, prev_thread) == 0);

    // schedule next thread
    assert(queue_dequeue(scheduled_q, (void**)&active_thread) == 0);
    uthread_ctx_switch(&prev_thread->ctx, &active_thread->ctx);
    preempt_enable();
  }

  preempt_disable();

  // collect target thread exit status from finished queue
  queue_iterate(finished_q, find_tcb, &tid, (void**)&target_t);

  // thread tid not found in any of the queues
  if (target_t == NULL)
    return -1;

  if(retval != NULL)
    *retval = target_t->exit_status;

  // remove collected thread from finished_q and free its resources
  queue_delete(finished_q, target_t);
  preempt_enable();

  uthread_ctx_destroy_stack(target_t->stack);
  free(target_t);
  
  return 0;
}

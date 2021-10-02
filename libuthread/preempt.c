#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */

// fire timer every 10000 microseconds
#define PERIOD 10000

struct itimerval old_timer;
struct sigaction old_action;

// signal handler for timer interrupt
void alarm_handler()
{
  uthread_yield();
}

// set up preemption
void preempt_start(void)
{
  struct itimerval new_timer = {};
  new_timer.it_value.tv_usec = PERIOD;
  new_timer.it_interval.tv_usec = PERIOD;

  struct sigaction new_action = {};

  // configure signal handler
  new_action.sa_handler = alarm_handler;
  sigaction(SIGVTALRM, &new_action, &old_action);

  // configure timer
  setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);
}

// restore previous timer configuration and signal action
void preempt_stop(void)
{
  sigaction(SIGVTALRM, &old_action, NULL);
  setitimer(ITIMER_VIRTUAL, &old_timer, NULL);
}

// unblock SIGVTALRM signal
void preempt_enable(void)
{
  sigset_t signal_set;
  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGVTALRM);
  sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
}

// block SIGVTALRM signal
void preempt_disable(void)
{
  sigset_t signal_set;
  sigemptyset(&signal_set);
  sigaddset(&signal_set, SIGVTALRM);
  sigprocmask(SIG_BLOCK, &signal_set, NULL);
}

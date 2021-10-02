#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue.h>

#define TEST_ASSERT(assert)        \
do {                  \
  printf("ASSERT: " #assert " ... ");  \
  if (assert) {            \
    printf("PASS\n");        \
  } else  {              \
    printf("FAIL\n");        \
    exit(1);            \
  }                  \
} while(0)

/* Create */
void test_create()
{
  fprintf(stderr, "\n*** TEST create ***\n");

  queue_t q = queue_create();
  TEST_ASSERT(q != NULL);
  TEST_ASSERT(queue_destroy(q) == 0);
}

/* Enqueue/Dequeue simple */
void test_queue_simple()
{
  int data = 3, *ptr;
  queue_t q;

  fprintf(stderr, "\n*** TEST queue_simple ***\n");

  q = queue_create();
  queue_enqueue(q, &data);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data);

  TEST_ASSERT(queue_destroy(q) == 0);
}

// enqueues 2, 3, 5, 8 and dequeues each one
void enqueue_simple()
{
  int data[] = {2, 3, 5, 8};
  int *ptr;

  fprintf(stderr, "\n*** TEST enqueue_simple ***\n");

  queue_t q = queue_create();

  for (int i = 0; i < 4; i++)
    queue_enqueue(q, &data[i]);

  TEST_ASSERT(queue_length(q) == 4);

  for (int i = 0; i < 4; i++) {
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data[i]);
  }

  TEST_ASSERT(queue_destroy(q) == 0);
}

//enqueue more data
void enqueue_more_data()
{ 
  int num = 15;
  int data[num];
  int *ptr;
  queue_t q = queue_create();

  fprintf(stderr, "\n*** TEST enqueue_more_data ***\n");
  
  for (int i = 0; i < num; i++) {
    data[i] = i;
  }

  for (int i = 0 ; i<num; i++) {
    queue_enqueue(q, &data[i]);
  }

  TEST_ASSERT(queue_length(q) == num);
  
  for (int i = 0; i < num; i++) {
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data[i]);
  }
  
  TEST_ASSERT(queue_destroy(q) == 0);
}

//enqueue different data type
void enqueue_char()
{
  fprintf(stderr, "\n*** TEST enqueue_char ***\n");

  char data[10] = "hello";
  int num = strlen(data);
  char *ptr;
  queue_t q = queue_create();
  
  for (int i = 0; i < num; i++){
    queue_enqueue(q, &data[i]);
  }
  
  TEST_ASSERT(queue_length(q) == num);
  
  for (int i =0; i < num; i++) {
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data[i]);
  }

  TEST_ASSERT(queue_destroy(q) == 0);
}

void delete_first()
{
  fprintf(stderr, "\n*** TEST delete_first ***\n");
  
  int data[] = {4, 5, 7};
  int *ptr;
  queue_t q = queue_create();

  for (int i = 0; i < 3; i++)
    queue_enqueue(q, &data[i]);

  TEST_ASSERT(queue_length(q) == 3);
  queue_delete(q, &data[0]);
  TEST_ASSERT(queue_length(q) == 2);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[1]);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[2]);

  TEST_ASSERT(queue_destroy(q) == 0);
}

void delete_middle()
{
  fprintf(stderr, "\n*** TEST delete_middle ***\n");
  
  int data[] = {4, 5, 7};
  int *ptr;
  queue_t q = queue_create();

  for (int i = 0; i < 3; i++)
    queue_enqueue(q, &data[i]);

  TEST_ASSERT(queue_length(q) == 3);
  queue_delete(q, &data[1]);
  TEST_ASSERT(queue_length(q) == 2);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[0]);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[2]);

  TEST_ASSERT(queue_destroy(q) == 0);
}

void delete_end()
{
  fprintf(stderr, "\n*** TEST delete_end ***\n");
  
  int data[] = {4, 5, 7};
  int *ptr;
  queue_t q = queue_create();

  for (int i = 0; i < 3; i++)
    queue_enqueue(q, &data[i]);

  TEST_ASSERT(queue_length(q) == 3);
  queue_delete(q, &data[2]);
  TEST_ASSERT(queue_length(q) == 2);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[0]);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[1]);

  TEST_ASSERT(queue_destroy(q) == 0);
}

// enqueue same memory address twice
// delete will extract the first occurrence only
void enqueue_same()
{
  fprintf(stderr, "\n*** TEST enqueue_same ***\n");
  
  int a = 23;
  int b = 5;
  int c = 20;
  int *ptr;
  queue_t q = queue_create();

  queue_enqueue(q, &a);
  queue_enqueue(q, &b);
  queue_enqueue(q, &c);
  queue_enqueue(q, &b);

  TEST_ASSERT(queue_length(q) == 4);
  queue_delete(q, &b);
  TEST_ASSERT(queue_length(q) == 3);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &a);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &c);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &b);

  TEST_ASSERT(queue_destroy(q) == 0);
}

void enqueue_multiple()
{
  fprintf(stderr, "\n*** TEST enqueue_multiple ***\n");

  int a = 2;
  int b = 4;
  int c = 5;
  int *ptr;
  
  queue_t q = queue_create();

  queue_enqueue(q, &a);
  queue_enqueue(q, &b);

  TEST_ASSERT(queue_length(q) == 2);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &a);
  
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &b);

  queue_enqueue(q , &c);
  TEST_ASSERT(queue_length(q) == 1);
  
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &c);

  TEST_ASSERT(queue_destroy(q) == 0);
}

// test enqueueing and dequeing multiple values of different types
void typed_enqueue_dequeue()
{
  fprintf(stderr, "\n*** TEST typed_enqueue_dequeue ***\n");

  float a = -4.5;
  int b = 23;
  long c = 51283;
  void* ptr;

  queue_t q = queue_create();

  queue_enqueue(q, &a);
  queue_enqueue(q, &b);
  queue_enqueue(q, &c);

  TEST_ASSERT(queue_length(q) == 3);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &a);

  char d = 'h';
  int e = 84;

  queue_enqueue(q, &d);
  queue_enqueue(q, &e);

  TEST_ASSERT(queue_length(q) == 4);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &b);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &c);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &d);
  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &e);

  TEST_ASSERT(queue_destroy(q) == 0);
}

// function that increments item by arg
// integer encoded as void*
static int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;
    (void)q; //unused

    *a += inc;

    return 0;
}

void test_iterator_inc()
{
  fprintf(stderr, "\n*** TEST test_iterator_inc ***\n");
  
  int data[] = {1, 2, 3, 4, 5};
  queue_t q = queue_create();
  int* ptr;

  for (int i = 0; i < 5; i++)
    queue_enqueue(q, &data[i]);
  
  // add 2 to each item in the queue
  queue_iterate(q, inc_item, (void*)2, NULL);

  TEST_ASSERT(data[0] == 3);
  TEST_ASSERT(data[1] == 4);
  TEST_ASSERT(data[2] == 5);
  TEST_ASSERT(data[3] == 6);
  TEST_ASSERT(data[4] == 7);

  for (int i = 0; i < 5; i++)
    queue_dequeue(q, (void**)&ptr);
  
  TEST_ASSERT(queue_destroy(q) == 0);
}

// returns the oldest node in the queue with a certain value
static int find_item(queue_t q, void *data, void *arg)
{
  int *a = (int*)data;
  int match = (int)(long)arg;
  (void)q; //unused

  if (*a == match)
      return 1;

  return 0;
}

void test_iterator_find() 
{
  fprintf(stderr, "\n*** TEST test_iterator_find ***\n");
  
  int data[] = {3, 6, 25, 8, 25};
  int *ptr = NULL;
  queue_t q = queue_create();

  for(int i = 0; i < 5; i++)
    queue_enqueue(q, &data[i]);

  queue_iterate(q, find_item, (void*)25, (void**)&ptr);
  TEST_ASSERT(ptr != NULL);
  TEST_ASSERT(*ptr == 25);
  TEST_ASSERT(ptr == &data[2]);

  TEST_ASSERT(queue_length(q) == 5);

  for (int i = 0; i < 5; i++)
    queue_dequeue(q, (void**)&ptr);
  
  TEST_ASSERT(queue_destroy(q) == 0);
}

// function that deletes odd nodes and multiplies even nodes by arg
static int odd_even(queue_t q, void *data, void *arg)
{
  int *a = (int*)data;
  int mult = (int)(long)arg;
  (void)q; //unused
  
  // delete odd nodes
  if (*a % 2 == 1)
    queue_delete(q, data);
  else
    *a *= mult;

  return 0;
}

void test_odd_even() 
{
  fprintf(stderr, "\n*** TEST test_odd_even ***\n");
  
  int data[] = {2, 3, 6, 7, 21, 14, 25};
  queue_t q = queue_create();
  int* ptr = NULL;

  for(int i = 0; i < 7; i++)
    queue_enqueue(q, &data[i]);

  queue_iterate(q, odd_even, (void*)2, NULL);

  TEST_ASSERT(queue_length(q) == 3);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[0]);
  TEST_ASSERT(*ptr == 4);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[2]);
  TEST_ASSERT(*ptr == 12);

  queue_dequeue(q, (void**)&ptr);
  TEST_ASSERT(ptr == &data[5]);
  TEST_ASSERT(*ptr == 28);

  TEST_ASSERT(queue_destroy(q) == 0);
}

void test_null_queue() 
{
  fprintf(stderr, "\n*** TEST test_null_queue ***\n");

  int x = 5;
  int *ptr;
  queue_t q = NULL;
  
  TEST_ASSERT(queue_destroy(q) == -1);
  TEST_ASSERT(queue_enqueue(q, &x) == -1);
  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);
  TEST_ASSERT(queue_delete(q, &x) == -1);
  TEST_ASSERT(queue_iterate(q, inc_item, (void*)2, NULL) == -1);
  TEST_ASSERT(queue_length(q) == -1);
}

void test_null_arguments()
{ 
  fprintf(stderr, "\n*** TEST test_null_arguments ***\n");
  
  int a = 20;
  int *ptr;
  queue_t q = queue_create();

  queue_enqueue(q, &a);
  
  TEST_ASSERT(queue_enqueue(q, NULL) == -1);
  TEST_ASSERT(queue_dequeue(q, NULL) == -1);
  TEST_ASSERT(queue_delete(q, NULL) == -1);
  TEST_ASSERT(queue_iterate(q, NULL, NULL, NULL) == -1);

  queue_dequeue(q, (void**)&ptr);

  TEST_ASSERT(queue_destroy(q) == 0);
}

void test_dequeue_empty()
{
  fprintf(stderr, "\n*** TEST test_dequeue_empty ***\n");
  
  int *ptr;
  int x = 5;

  queue_t q = queue_create();

  TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);
  TEST_ASSERT(queue_delete(q, &x) == -1);

  TEST_ASSERT(queue_destroy(q) == 0);
}

void test_delete_not_found()
{
  fprintf(stderr, "\n*** TEST test_delete_not_found ***\n");

  int data[] = {2, 3, 5, 8};
  int val = 8;
  int *ptr = &val;

  queue_t q = queue_create();

  for (int i = 0; i < 4; i++)
    queue_enqueue(q, &data[i]);

  // memory address not stored in queue
  TEST_ASSERT(queue_delete(q, &val) == -1);

  // destroy non_empty queue
  TEST_ASSERT(queue_destroy(q) == -1);

  for (int i = 0; i < 4; i++)
    queue_dequeue(q, (void**)&ptr);

  TEST_ASSERT(queue_destroy(q) == 0);
}

int main(void)
{
  test_create();
  test_queue_simple();
  enqueue_simple();
  delete_first();
  delete_middle();
  delete_end();
  enqueue_same();
  enqueue_multiple();
  test_iterator_inc();
  test_iterator_find();
  typed_enqueue_dequeue();
  test_odd_even();
  enqueue_more_data();
  enqueue_char();
  test_null_queue();
  test_null_arguments();
  test_dequeue_empty();
  test_delete_not_found();

  return 0;
}

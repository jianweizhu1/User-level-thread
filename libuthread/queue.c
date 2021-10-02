#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct Node {
  struct Node* next;
  void* data;
} Node;

struct queue {
  int size;
  Node* head;
  Node* tail;
};  

queue_t queue_create(void) {
  return calloc(1, sizeof(struct queue));
}

int queue_destroy(queue_t queue)
{
  if (!queue || queue->size != 0)       // null or empty queue
    return -1;

  free(queue);
  return 0;
}

// enqueue an item to end of the list
int queue_enqueue(queue_t queue, void *data)
{
  if(!queue || !data)
    return -1;

  Node* new_node = malloc(sizeof(Node));
  new_node->next = NULL;
  new_node->data = data;

  if(!new_node)
    return -1;
  
  // empty list
  if(!queue->head) {
    queue->head = new_node;
    queue->tail = new_node;
  }
  
  // insert at the end
  else {
    queue->tail->next = new_node;
    queue->tail = new_node;
  }

  queue->size++;

  return 0;
}

// dequeue a node from the front of the list
int queue_dequeue(queue_t queue, void **data)
{
  if (!queue || queue->size == 0 || !data)
    return -1;

  Node* front_node = queue->head;
  Node* next_node = front_node->next;

  // get data of first node
  *data = front_node->data;

  free(front_node);
  queue->head = next_node;

  queue->size--;

  if(queue->size == 0)
    queue->tail = NULL;

  return 0;
}

// delete the first occurence of item with the matching address
int queue_delete(queue_t queue, void* data)
{ 
  if (!queue || queue->size == 0 || !data)
    return -1;

  Node* node = queue->head;

  // delete first node
  void* ptr = NULL;
  if(node->data == data) 
    return queue_dequeue(queue, &ptr);
  
  // otherwise, search for node to delete
  while(node->next) {
    if(node->next->data == data) {        // match found
      Node* new_node = node->next->next;
      free(node->next);
      node->next = new_node;

      if(!node->next)                     // no next node
        queue->tail = node;

      queue->size--;
      return 0;
    }   
    node = node->next;
  }

  return -1;
}

// calls func on each item in the queue
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
  if (!queue || !func)
    return -1;

  Node* node = queue->head;

  while(node) {
    Node* next_node = node->next;
    
    // stop iteration at this item and get the node's data
    if (func(queue, node->data, arg) == 1) {
      if(data)
        *data = node->data;

      return 0;
    }

    node = next_node;
  }

  return 0;
}

int queue_length(queue_t queue)
{
  if(!queue)
    return -1;
  
  return queue->size;
}

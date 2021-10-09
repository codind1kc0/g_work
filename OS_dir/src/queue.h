#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

typedef  list_node    queue_node;
typedef  struct{
    list_node    head;
	int length;
}t_queue;

void  queue_init(t_queue*  queue);
int   queue_is_empty(t_queue* queue);
int   queue_is_contained(t_queue* queue, queue_node* node);
void  queue_add(t_queue* queue, queue_node* node);

queue_node*  queue_front(t_queue* queue);
queue_node*  queue_remove(t_queue* queue);

int   queue_length(t_queue* queue);
void  queue_rotate(t_queue* queue);

#endif

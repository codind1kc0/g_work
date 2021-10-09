#include "queue.h"

void  queue_init(t_queue*  queue)
{
    list_init((t_list* )queue);
	queue -> length = 0;
}

int   queue_is_empty(t_queue* queue)
{
    return  list_is_empty((t_list* )queue);
}

int   queue_is_contained(t_queue* queue, queue_node* node)
{
    queue_node* tnode = NULL;
	
	list_for_each(&queue -> head, tnode) {
	    if (1 == IsEqual(tnode, node)) {
		    return 1;
		} else {
		    continue;
		}
	}

	return 0;
}

void  queue_add(t_queue* queue, queue_node* node)
{
    list_add_tail((t_list* )queue, node);
	++queue -> length;
}

queue_node*  queue_front(t_queue* queue)
{
    queue_node*  qnode = NULL;

    if (0 == queue_is_empty(queue)) {
	    qnode = queue -> head.next;
	}

	return qnode; 
}

queue_node*  queue_remove(t_queue* queue)
{
    queue_node* qnode = NULL;

    if (queue_length(queue) > 0) {
        if (NULL != (qnode = queue_front(queue)) ) {
	        del_node(qnode);
	        --queue -> length;
	     } 
	}

	return qnode;

}

int   queue_length(t_queue* queue)
{
    return queue -> length;
}

void  queue_rotate(t_queue* queue)
{
    queue_node* chg_node = NULL;
    
    if (NULL != (chg_node = queue_remove(queue))) {
	    queue_add(queue, chg_node);
	}

}



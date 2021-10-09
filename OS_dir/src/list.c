#include "list.h"

static void _list_add(list_node* node, list_node* prev, list_node* next)
{
    node -> next = next;
	node -> prev = prev;

    next -> prev = node;
	prev -> next = node;
}

static void _list_del(list_node* prev, list_node* next)
{
    prev -> next = next;
	next -> prev = prev;
}

void  list_init(t_list* list)
{
    list -> next = list;
	list -> prev = list;
}

void  list_add(t_list* list, list_node* node)
{
#if 0
    node -> prev = list;
	node -> next = list -> next;

    list -> next -> prev = node;
	list -> next = node;
#endif
    _list_add(node, list, list->next);
} //insert to list head

void  list_add_tail(t_list* list, list_node* node)
{
#if 0
    node -> prev = list -> prev;
	node -> next = list -> prev -> next;

	list -> prev -> next = node;
	list -> prev = node;
#endif
    _list_add(node, list->prev, list);
} //insert to list tail

void  list_add_before(list_node* o_node, list_node* n_node)
{
#if 0
    n_node -> prev = o_node -> prev;
    n_node -> next = o_node;

	o_node -> prev -> next = n_node;
	o_node -> prev = n_node;
#endif
    _list_add(n_node, o_node->prev, o_node);
}// insert n_node before o_node

void  list_add_after(list_node* o_node, list_node* n_node)
{
#if 0
    n_node -> prev = o_node;
	n_node -> next = o_node -> next;

	o_node -> next -> prev = n_node;
	o_node -> next = o_node;
#endif
    _list_add(n_node, o_node, o_node -> next);
} // either


void  del_node(list_node* node)
{
#if 0
    node -> prev -> next = node -> next;
	node -> next -> prev = node -> prev;
#endif 
    _list_del(node -> prev, node -> next);
	node -> next = NULL;
	node -> prev = NULL;
}

void  replace_node(list_node* o_node, list_node* n_node)
{
#if 0
    n_node -> prev = o_node -> prev;
	n_node -> next = o_node -> next;

	o_node -> prev -> next = n_node;
	o_node -> next -> prev = n_node;
#endif
    _list_del(o_node -> prev, o_node -> next);
	_list_add(n_node, o_node -> prev, o_node -> next);

	o_node -> next = NULL;
	o_node -> prev = NULL;
}

int  list_is_last(t_list* list, list_node* node)
{
    return IsEqual(list, node->next);
}


int  list_is_empty(t_list* list)
{
    return (IsEqual(list, list->next) && IsEqual(list, list->prev));
}

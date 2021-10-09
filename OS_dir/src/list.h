#ifndef  LIST_H
#define  LIST_H

#include "const.h"
#include "utility.h"

typedef struct _list_node{
    struct _list_node*  next;
	struct _list_node*  prev;
}list_node;

typedef list_node   t_list;

#define  list_for_each(list, pos)     for(pos=((list)->next); !IsEqual(list, pos); pos = pos->next)
#define  get_list_node(ptr, type, member) Container(ptr, type, member)

void  list_init(t_list* list);
void  list_add(t_list* list, list_node* node); //insert to list head
void  list_add_tail(t_list* list, list_node* node); //insert to list tail
void  list_add_before(list_node* o_node, list_node* n_node);// insert n_node before o_node
void  list_add_after(list_node* o_node, list_node* n_node); // either

void  del_node(list_node* node);
void  replace_node(list_node* o_node, list_node* n_node);

int  list_is_last(t_list* list, list_node* node);
int  list_is_empty(t_list* list);

#endif 

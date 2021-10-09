#ifndef  KMUTEX_H
#define  KMUTEX_H

#include "type.h"
#include "list.h"
#include "memory.h"
#include "task.h"

typedef struct  mutex_type_{
    list_node  head;
	uint       type;
	uint       lock;
	t_queue    wait; 
}mutex_type;

enum {
    nomal, strict
};

void mutex_module_init(void);
void mutex_call_handler(uint cmd, uint param1, uint param2); //other param in registor.

#endif

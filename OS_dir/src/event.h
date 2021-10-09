#ifndef  EVENT_H
#define  EVENT_H

#include  "type.h"
#include  "memory.h"

enum {
    NoneEvent,
    MutexEvent,
	KeyEvent,
	TaskEvent,
};

typedef struct event_ {
    uint type;
	uint id; 
	uint param1;
	uint param2;
}event_t;
				

event_t*  create_event(uint type, uint id, uint param1, uint param2);
void      destory_event(event_t* event);

#endif

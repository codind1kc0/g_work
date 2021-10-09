#include "event.h"

event_t* create_event(uint type, uint id, uint param1, uint param2)
{
    event_t* ret = (event_t* )m_malloc(sizeof (event_t));
    
	if (ret) {
		ret -> type   = type;
		ret -> id     = id;
		ret -> param1 = param1;
		ret -> param2 = param2;
    }

	return ret;
}

void destory_event(event_t* event)
{
    m_free((void*)event);
}


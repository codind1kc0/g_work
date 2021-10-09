#ifndef  APP_H
#define  APP_H

#include "type.h"
#include "utility.h"


typedef struct  app_info_ {
    const char*  name;
	void  (*tmain)(void);
	ushort  priority;
	char   extern_[0];
}app_info;

#if 0
void  app_mod_init(void);
app_info* get_app_run(uint  index);
uint      get_app_num(void);
#endif

#endif

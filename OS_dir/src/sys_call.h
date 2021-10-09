#ifndef  SYS_CALL_H
#define  SYS_CALL_H

#include "type.h"
#include "mutex.h" 

void  sys_exit(void);

// mutex ID in the  uint type val.
uint  create_mutex(uint type);
void  enter_critical(uint mutex);
void  exit_critical(uint mutex);
int   destory_mutex(uint mutex);

void  wait(const char*  task_name);

void  reg_app(const char* name, void (*entry)(void), byte pri);

uint read_key(void);

#endif

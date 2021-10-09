#include "sys_call.h" 

#define  SysCall(type, cmd, param1, param2)   asm volatile (\
                                          	  "movl $"#type", %%eax\n\t"\
											  "movl $"#cmd ", %%ebx\n\t"\
											  "movl %0, %%ecx\n\t"\
											  "movl %1, %%edx\n\t"\
											  "int      $0x80\n\t"\
	                            ::"r"(param1), "r"(param2):"eax", "ebx", "ecx", "edx")


uint create_mutex(uint type)
{
    volatile uint  ret;

    SysCall(1, 0, &ret, type);

	return ret;
}

void  enter_critical(uint mutex)
{
    volatile  uint    wait = 0;
    
	do 
	{
     	SysCall(1, 1, mutex, &wait);
    } while (wait);
}

void  exit_critical(uint mutex)
{
    SysCall(1, 2, mutex, 0);   
}

int  destory_mutex(uint mutex)
{
    int ret = 0;

	SysCall(1, 3, mutex, &ret);

	return ret;
}

/* paramer :user mode ==> kernel mode */
void  sys_exit(void)
{
   SysCall(0, 0, 0, 0);
}


void  wait(const char*  task_name)
{
    SysCall(0, 1, task_name, 0);
}

void reg_app(const char* name, void (*entry)(void), byte priority)
{
   app_node info = {0};
        
   if ( name && entry  ) { 
       info. app.name  = name;
       info. app.tmain = entry;
       info. app.priority = priority;
								        
       SysCall(0, 2, &info, 0); 										                    
   }   
}

uint read_key(void)
{
    uint ret = 0;
    SysCall(2, 0, &ret, 0);

	return ret;
}


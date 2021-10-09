#ifndef  TASK_H
#define  TASK_H

#include "kernel.h"
#include "queue.h"
#include "app.h"
#include "event.h"
#include "mutex.h"

#define  MAX_TASK              32 
#define  MAX_RUNNING_TASK      16 
#define  MAX_TASK_BUFFER_NUM   (MAX_TASK +  1)
#define  MAX_READY_TASK        (MAX_TASK - MAX_RUNNING_TASK)

//more timer pice. task don't moved early and rotate "Bdie" some task
#define  MAX_PICE              260

#define  TASK_STACK_BASE       0x40000
#define  TASK_STACK_SIZE       0x200

#define  PID_BASE              0x10

typedef struct reg_value_{
    uint    gs;
    uint    fs;
    uint    es;
    uint    ds;

    uint    edi;
    uint    esi;
    uint    ebp;
    uint    kesp;
    uint    ebx;
    uint    edx;
    uint    ecx;
    uint    eax;

    uint    err_code;
    uint    eip;
    uint    cs;
    uint    eflags;
    uint    esp;
    uint    ss;

}reg_value;

typedef struct tss_ {
    uint    previous;
    uint    esp0;
    uint    ss0;
    
    uint    unused[22];
    ushort  reserved;
    ushort  iomap;
}TSS;

typedef struct task_{
    reg_value  rv; // 18*4
    descriptor ldt[3]; //  3*8
//    TSS        tss; // 104 
    ushort     ldt_selector; // off:50*4
    ushort     tss_selector;
//    task running entry
    void       (* tmain)(void);
    
//    running level
	byte       current;
	ushort     total;

    uint       id;
    char       name[16];
	t_queue    wait_queue;
    byte*      stack;  //byte       stack[512];

//  event !!!
    event_t*   event;
}task;

typedef struct task_node_{
    queue_node  head_;
	task        task_;
}task_node;

typedef struct 
{
    queue_node head;
	app_info   app;
}app_node;

enum {
    NOTIFY, WAIT
};

extern  void (* volatile const pload_task)(volatile task* p);
extern  void (* volatile const p_gfunc)(volatile task* pt);

//extern  task_node  gTaskBuffer[4];

static void  init_task(task* pt, const char* name, uint pid, void (* task_entry)(void), ushort pri);
void  task_mod_init(void);
void  switch_task(void);
void  launch_task(void);

void  event_schedule(uint action, event_t* event);
void  mx_schedule(uint action);

void  task_sys_deal(uint cmd, uint param1, uint param2);

#endif

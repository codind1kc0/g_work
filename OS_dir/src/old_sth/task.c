#include "utility.h"
#include "task.h"

void (* volatile const pload_task)(volatile task* p) = NULL;
void (* volatile const p_gfunc)(volatile task* pt) = NULL;

volatile task* p_task = NULL;

static TSS g_TSS = {0};

//static task_node  gTaskBuffer[MAX_TASK]; can't use kernel space. move to 0x40000
static task_node gTaskBuffer[MAX_TASK_BUFFER_NUM] = {0};  
static t_queue    gAppToRun    = {0};
static t_queue    gRunningTask = {0};
static t_queue    gFreeTask    = {0};
static t_queue    gReadyTask   = {0};
//wait: mutex, ...
static t_queue    gWaitTask    = {0};   

// sys running first task.
//static task_node  gIdleTask    = {0}; can't use kernel space. 
static task_node*   gIdleTask = NULL;
static uint  gpid         = PID_BASE;

static void app_info_to_run(const char* name, void (*entry)(void), byte pri)
{
    app_node* an = (app_node* )m_malloc(sizeof(app_node));

    if ( an ) {
	    char* s = name ? (char*)m_malloc(str_len(name) + 1) : NULL;

        an -> app.name = s ? str_cpy(s, name, -1) : NULL;
		an -> app.tmain = entry;
		an -> app.priority = pri;
	
	    queue_add(&gAppToRun, (queue_node*)an);
	}
}


static void  run_task_entry()
{
    if ( p_task ) {
	    (*(p_task -> tmain))();
	}

	// some destory working...
	asm volatile (
	"movl  $0x00, %eax\n\t"
	"int   $0x80      \n\t"
	);
	// and schedule next task running...
	//while (1);
}

static void create_task(void)
{

	while  (  (0<queue_length(&gAppToRun)) &&  (queue_length(&gReadyTask) < MAX_READY_TASK) ) {
	    task_node* tn = (task_node* )queue_remove(&gFreeTask);

		if (tn) {
		    app_node* an = (app_node*)queue_remove(&gAppToRun);   
			init_task(&(tn -> task_), an->app.name, gpid++, an->app. tmain, an -> app.priority);
			queue_add(&gReadyTask, (queue_node*)tn);
     	   
		   m_free((void* ) an->app.name);
		   m_free((void* ) an);
		} else {
		    break;
		}   
    }
}

static void  ready_to_running(void)
{ 
    queue_node* pnode = NULL;

	if ( queue_length(&gReadyTask) < MAX_READY_TASK ) {
	   create_task(); 
	} 

	while ( (queue_length(&gReadyTask) > 0 ) &&\
	    (queue_length(&gRunningTask) < MAX_RUNNING_TASK) ) {
	    pnode = queue_remove(&gReadyTask);
	    ((task_node* )pnode) -> task_ .current = 0;
        queue_add(&gRunningTask, pnode);
	}

}


static void app_main_to_run(void)
{
	app_info_to_run("AppMain", (void* )(*(uint* )APP_MAIN_ENTRY), 200);

}



static void  running_to_wait(t_queue* wait_queue)
{
    queue_node*  pnode = queue_front(&gRunningTask);
    
	if ( !IsEqual(gIdleTask, pnode) ) {
	    pnode = queue_remove(&gRunningTask);
	    queue_add(wait_queue, pnode);
	}
}

static void  wait_to_ready(t_queue* rwait_queue)
{
    queue_node* pnode = NULL;

    while ( !queue_is_empty(rwait_queue) ) {
	    pnode = queue_remove(rwait_queue);
	    queue_add(&gReadyTask, pnode);
    }
}

static void  running_to_ready(void)
{
   task_node* tn = (task_node* )queue_front(&gRunningTask);
   
   // when running task equal one... need protect
   if (queue_length(&gRunningTask) > 0) {
       if (!IsEqual(tn, gIdleTask)) {
           if ((tn -> task_).total == (tn -> task_).current) {
	           queue_remove(&gRunningTask);
		       queue_add(&gReadyTask, (queue_node* )tn);
	       }
       }
   }
}

static  void  check_running_task(void)
{
    if ( 0 == queue_length(&gRunningTask) ) {
        queue_add(&gRunningTask, (queue_node* )gIdleTask);	
	} else {
	    if ( (queue_length(&gRunningTask) > 1) && IsEqual(queue_front(&gRunningTask), gIdleTask) ) {
		    queue_remove(&gRunningTask);
		}
	}
}

static task_node* find_task_by_name(const char* name)
{
    int i = 0; 
    task_node* fnd = gTaskBuffer;
	task_node* ret = NULL;

    if ( !str_cmp(name, "IdleTask", -1) ) {
    	for ( i=0; (!ret) && (i<MAX_TASK_BUFFER_NUM);  i++ ) {
	    	if ( fnd[i].task_.id ) {
		   		ret = (str_cmp(fnd[i].task_.name, name, -1) ? (fnd+i) : NULL);
			}
		}
	}
    
	return ret;
}


void IdleTask(void)
{
// nothing to do, only loop.
    while (1) 
	{
	}
}

static void init_task(task* pt, const char* name, uint pid, void (* task_entry)(void), ushort pri)
{
    
    pt->rv.cs = LDT_CODE32_SELECTOR;
    pt->rv.ds = LDT_DATA32_SELECTOR;
    pt->rv.fs = LDT_DATA32_SELECTOR;
    pt->rv.es = LDT_DATA32_SELECTOR;
    pt->rv.ss = LDT_DATA32_SELECTOR;
    pt->rv.gs = LDT_VIDEO_SELECTOR;
     

    pt->rv.esp = (uint)pt->stack + TASK_STACK_SIZE;  //sizeof(pt->stack);
    pt->rv.eip = (uint)run_task_entry ;
    pt->rv.eflags = 0x3202;

	pt->tmain = task_entry;
	
	if ( name ) {
        str_cpy((pt->name), name, sizeof(pt->name) - 1);
	} else {
	    *(pt->name) = 0;
	}

    pt -> id = pid;

	/* queue must init before used!  */
    queue_init(&pt -> wait_queue);

	pt -> total = MAX_PICE - pri; // (260 - priority) mapping timer chip.

  // print_int_hex(queue_length(&gReadyTask));
    pt -> current = 0;

    set_descriptor_value(AddressOff( pt->ldt, LDT_VIDEO_INDEX ),  0xB8000, 0x07FFF, DA_DRWA + DA_32 + DA_DPL3);
    set_descriptor_value(AddressOff( pt->ldt, LDT_CODE32_INDEX ), 0x0000, KERNEL_HEAP_BASE - 1, DA_C + DA_32 + DA_DPL3);
    set_descriptor_value(AddressOff( pt->ldt, LDT_DATA32_INDEX ), 0x0000, KERNEL_HEAP_BASE - 1, DA_DRW + DA_32 + DA_DPL3);
    
    pt->ldt_selector = GDT_TASK_LDT_SELECTOR;
    pt->tss_selector = GDT_TASK_TSS_SELECTOR;

// set gdt_ldt and tss moved..
}

static void prepare_task_running(task* pt) 
{
    pt -> current ++;
    // stack info
    g_TSS.ss0 = GDT_DFLAT32_SELECTOR;
    // tss save esp0; esp0 first pointer context(registor) bottom.
    g_TSS.esp0 = (uint)&pt->rv + sizeof(pt->rv);
    g_TSS.iomap = sizeof(TSS);

    set_descriptor_value(AddressOff( Ggdt_info.entry, GDT_TASK_LDT_INDEX ), (uint)&pt->ldt, sizeof(pt->ldt)-1, DA_LDT + DA_DPL0);
}
#ifndef   SCHEDULE_FUNC
#define   SCHEDULE_FUNC

void schedule(void)
{
// p_task value equal &p/&t , but can't ...
	
	running_to_ready();
	ready_to_running();
    check_running_task();

//A runnning over(to short). B was moved
	queue_rotate(&gRunningTask);
	prepare_task_running(&(((task_node* )queue_front(&gRunningTask)) -> task_));
	    

    p_task = &(((task_node* )queue_front(&gRunningTask)) -> task_);

    g_TSS.ss0 = GDT_DFLAT32_SELECTOR;
    g_TSS.esp0 = (uint)&p_task->rv.gs + sizeof(reg_value);
    set_descriptor_value(AddressOff( Ggdt_info.entry, GDT_TASK_LDT_INDEX ), (uint)&p_task->ldt, sizeof(p_task->ldt)-1, DA_LDT + DA_DPL0);

    // run_task
    (* pload_task)(p_task);
}

static void schedule_next(void)
{
    ready_to_running();
   	check_running_task();
   	queue_rotate(&gRunningTask);
 	prepare_task_running(&(((task_node* )queue_front(&gRunningTask)) -> task_));
	p_task = &(((task_node* )queue_front(&gRunningTask)) -> task_);
}

void mx_schedule(uint action)
{
    switch (action) 
	{
	case  NOTIFY:
	    wait_to_ready((t_queue*)&gWaitTask);
		break;

	case  WAIT:
        running_to_wait((t_queue*)&gWaitTask);
        schedule_next();
        break;

	default:break;
	}
}

#endif



void task_mod_init(void)
{
    int i = 0;

// total used ( MAX_TASK + 1 )*sizeof (task_node) bytes.
    byte* pstack = (byte* )(APP_HEAP_BASE - (MAX_TASK_BUFFER_NUM * TASK_STACK_SIZE));
	for (i = 0; i < MAX_TASK_BUFFER_NUM; i++) {
         task_node* pt = (task_node* )AddressOff(gTaskBuffer, i);
		 pt -> task_.stack = AddressOff(pstack, (TASK_STACK_SIZE * i));	    
	}

	gIdleTask   = (void* )AddressOff(gTaskBuffer, MAX_TASK);  
    
    queue_init(&gAppToRun);
	queue_init(&gFreeTask);
	queue_init(&gReadyTask);
	queue_init(&gWaitTask);
	queue_init(&gRunningTask);

	for (i = 0; i < MAX_TASK; i++) {
        queue_add(&gFreeTask, (queue_node* )AddressOff(gTaskBuffer, i));
	}

    // initalize shared TSS
	set_descriptor_value(AddressOff( Ggdt_info.entry, GDT_TASK_TSS_INDEX ), (uint)&g_TSS, sizeof(g_TSS)-1, DA_386TSS + DA_DPL0);
   
    
    init_task(&(gIdleTask -> task_), "Idle", gpid++, &IdleTask, 255);
    //queue_add((t_queue* )&gRunningTask, (queue_node* )&gIdleTask); 
	//schedule first user task running...
	app_main_to_run();

	ready_to_running();

    check_running_task();
}

void launch_task(void)
{
    // that is very important pointor.
	//print_int_hex((uint)&(gTaskBuffer[index].task_));
	//print_string("      ");
    //print_int_hex((uint)&( ((task_node* )AddressOff(gTaskBuffer, index))->task_ ));
	
    prepare_task_running(&(((task_node* )queue_front(&gRunningTask)) -> task_));

    p_task = &(((task_node* )queue_front(&gRunningTask)) -> task_);
    (*p_gfunc)(p_task);
}

/* sys call handle  */
static void kill_task(void)
{
    task_node* run_task = (task_node* )queue_remove( (t_queue*)&gRunningTask);
	run_task -> task_.id = 0;

	wait_to_ready( &run_task->task_.wait_queue );
	queue_add(&gFreeTask, (queue_node*)run_task );
   
	schedule();
}

static void wait_task(const char*  name)
{
    task_node*  add_task = find_task_by_name(name);

	if ( add_task ) {
	    running_to_wait( &(add_task->task_.wait_queue) );
        schedule_next();
	}
}

void task_sys_deal(uint cmd, uint param1, uint param2)
{
    switch (cmd)
	{
	case 0:
	    kill_task();
		break;
	case 1:
	    wait_task((const char* )param1);
		break;
	case 2:
	    app_info_to_run( ((app_node*)param1)->app.name, ((app_node*)param1)->app.tmain,\
		                 ((app_node*)param1)->app.priority);
		break;

	default: break;
	} 
}

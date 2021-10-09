#include "utility.h"
#include "task.h"
#include "screen.h"

//void (* volatile const pload_task)(volatile task* p) = NULL;
void (* volatile const p_gfunc)(volatile task* pt) = NULL;

volatile task* p_task = NULL;

static TSS g_TSS = {0};

//static task_node  gTaskBuffer[MAX_TASK]; can't use kernel space. move to 0x40000
static task_node gTaskBuffer[MAX_TASK_BUFFER_NUM] = {0};  
static t_queue    gAppToRun    = {0};
static t_queue    gRunningTask = {0};
static t_queue    gFreeTask    = {0};
static t_queue    gReadyTask   = {0};

// sys running first task.
static task_node*   gIdleTask = NULL;
static uint  gpid         = PID_BASE;


static task* get_current_task(void)
{
    return (task*)p_task;
}

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
#if 0
    task* run = NULL;
    if  (NULL != ( run = get_current_task() )) {
        (*(run -> tmain))();
    }
#endif
    (*(p_task->tmain))();

    // some destory working...
    asm volatile (
    "movl  $0x00, %eax\n\t"
    "int   $0x80      \n\t"
    );
    // and schedule next task running...
    
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

    if (queue_length(&gReadyTask) < MAX_READY_TASK ) {
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
    task_node* pnode = NULL;

    while ( !queue_is_empty(rwait_queue) ) {
        pnode = (task_node*)queue_remove(rwait_queue);
        
        destory_event(pnode->task_.event);
        pnode->task_.event = NULL;

        queue_add(&gReadyTask, (queue_node*)pnode);
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
    pt->event = NULL;
    
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
    ++pt -> current;
    // stack info
    g_TSS.ss0 = GDT_DFLAT32_SELECTOR;
    // tss save esp0; esp0 first pointer context(registor) bottom.
    g_TSS.esp0 = (uint)&pt->rv + sizeof(pt->rv);
    g_TSS.iomap = sizeof(TSS);

    set_descriptor_value(AddressOff( Ggdt_info.entry, GDT_TASK_LDT_INDEX ), (uint)&pt->ldt, sizeof(pt->ldt)-1, DA_LDT + DA_DPL0);
}

#define   SCHEDULE_FUNC

static void schedule_next(void)
{
    ready_to_running();
    check_running_task();

    queue_rotate(&gRunningTask);
    p_task = &(((task_node* )queue_front(&gRunningTask)) -> task_);
	
    prepare_task_running((task*)p_task);
}


void schedule(void)
{
    running_to_ready();
    schedule_next();
}

static void  wait_event(t_queue* wait, event_t* event)
{
    p_task -> event = event;
    running_to_wait(wait);
    schedule_next();
}

static void  mutex_schedule(uint action, event_t* event)
{
    mutex_type* mx =  (mutex_type*)event->id;
    
    if ( action==NOTIFY ) {
        wait_to_ready(&mx->wait);
    } else if ( action==WAIT ){
        wait_event(&mx->wait, event);
    }

}

static void  keyboard_schedule(uint action, event_t* evt)
{
    t_queue* w_task = (t_queue*)evt->id;

    if (NOTIFY == action) {
        uint kc = evt->param1;
        list_node* pos = NULL;

        list_for_each((t_list*)w_task, pos) {
            task_node* tn = (task_node*)pos; 
            uint* ret =  (uint*)(tn->task_.event->param1);
            *ret = kc;
        }

        wait_to_ready(w_task);
    } else if (WAIT == action){
        wait_event(w_task, evt);     
    }
}

static void  task_schedule(uint action, event_t* event)
{
/*  task status must changed  */
    task_node* tn = (task_node*)(event->id);
    
    if (  action==NOTIFY ) {
       //set_print_pos(CMD_PINFO_W, CMD_PINFO_H + 7);
       //print_int_hex(queue_length(&tn->task_.wait_queue));
        wait_to_ready(&(tn->task_.wait_queue));
    } else if ( action==WAIT ) {
        wait_event(&(tn->task_.wait_queue), event);    
    }
}

void event_schedule(uint action, event_t* event)
{
    switch( event->type ) {
        case  NoneEvent:
        // nothing to do...
            break;

        case  MutexEvent:
            mutex_schedule(action, event);
            break;

        case  KeyEvent:
            keyboard_schedule(action, event);
            break;

        case  TaskEvent:
            task_schedule(action, event);
            break;

        default: break;
                        
    }
}



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
    prepare_task_running(&(((task_node* )queue_front(&gRunningTask)) -> task_));

    p_task = &(((task_node* )queue_front(&gRunningTask)) -> task_);
    (*p_gfunc)(p_task);
}

/* sys call handle  */
static void kill_task(void)
{
    task_node* run_task = (task_node* )queue_front( (t_queue*)&gRunningTask);

    event_t* evt = create_event(TaskEvent, (uint)run_task, 0, 0);
    event_schedule(NOTIFY, evt);
    run_task -> task_.id = 0;

    queue_remove((t_queue*) &gRunningTask);
    queue_add(&gFreeTask, (queue_node*)run_task);

    schedule();
}

static void wait_task(const char*  name)
{
    task_node*  add_task = find_task_by_name(name);

    if ( add_task ) {
        event_t* evt = create_event(TaskEvent, (uint)add_task, 0, 0);

        if (evt) {
            event_schedule(WAIT, evt);
        }
    }
}

void task_sys_deal(uint cmd, uint param1, uint param2)
{
    switch (cmd)
    {
    case 0:
        kill_task();
		delay(10);
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

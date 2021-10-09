#include "mutex.h"

extern  volatile  task*     p_task;
static  t_list    gMutexlist = {0};

static inline mutex_type* 
sys_mutex_create(uint type)
{
    mutex_type*  ret = NULL;
	
	ret = (mutex_type* )m_malloc(sizeof (mutex_type));
    if (ret) {
	    queue_init(&ret->wait);
	    ret -> lock = 0;
		ret -> type = type; 
		list_add(&gMutexlist, (list_node* )ret);
		//print_string("create mutex.\n");
    }

	return ret;
}

static inline  int 
mutex_in_sys(mutex_type* mutex)
{
    int  ret = 0;
    list_node* pos = NULL;
    
	list_for_each(&gMutexlist, pos) {
	    if (IsEqual(pos, (list_node* )mutex)) {
		    ret = 1;
			break;
		}
    }
	return ret;
}


// new: do_wait()
static inline  void  do_wait(mutex_type* mutex, uint* wait)
{
    event_t* evt = create_event(MutexEvent, (uint)mutex, 0, 0);

	if ( evt ) {
	    event_schedule(WAIT, evt);
	    *wait = 1;
	}
}

static inline  void
strict_enter_critical(mutex_type* mutex, uint* wait)
{
    if (mutex && mutex_in_sys(mutex)) {
        if (0 == mutex -> lock) {
		    mutex -> lock = (uint)p_task;
			*wait = 0;
		} else if ( IsEqual(mutex->lock, (uint)p_task) ) {
		    *wait = 0;
		} else {
	//	    mx_schedule(WAIT);
	//		*wait = 1;
	        do_wait(mutex, wait);
		}
	}	
}

static inline  void
nomal_enter_critical(mutex_type* mutex, uint* wait)
{
    if (mutex && mutex_in_sys(mutex)) {
        if (0 == mutex -> lock) {
		    mutex -> lock = 1;
			*wait = 0;
		} else {
		    //mx_schedule(WAIT);
			//*wait = 1;
	        do_wait(mutex, wait);
		}
	}	
}



static inline  void
sys_enter_critical(mutex_type* mutex, uint* wait)
{
    if (mutex && mutex_in_sys(mutex)) {
        switch( mutex->type ) {
		case nomal:
		    nomal_enter_critical(mutex, wait);
			break;
	    case strict:
		    strict_enter_critical(mutex, wait); 
            break;
		default:
		    break; 
		    
		} 
	}	
}

static inline  void
nomal_exit_critical(mutex_type* mutex)
{
// NOTIFY event
    event_t* evt = create_event(MutexEvent, (uint)mutex, 0, 0);
	mutex -> lock = 0;

	event_schedule(NOTIFY, evt);
}

static inline  void
strict_exit_critical(mutex_type* mutex)
{
    if (mutex && mutex_in_sys(mutex)) {
        if ( IsEqual(mutex->lock, (uint)p_task) ) {
		    nomal_exit_critical(mutex);
	    } else {
	        print_string("OOPS!\n");
			sys_call_handler(0, 0, 0, 0);
	    }
	}
}



static inline  void
sys_exit_critical(mutex_type* mutex)
{
    if (mutex && mutex_in_sys(mutex)) {
        switch (mutex->type) {
		case  nomal:
		    nomal_exit_critical(mutex);
		    break;
		case  strict:
		    strict_exit_critical(mutex);
		    break;
		default:
		    break;
		}
    }
}

static inline  int  
sys_destory_mutex(mutex_type* mutex)
{
    int ret = 0;
	list_node* pos = NULL;
     
    if ( IsEqual(mutex-> lock, 0)  ) {
		list_for_each(&gMutexlist, pos) {
	    	if (IsEqual(pos, (list_node* )mutex)) {
		   		del_node((list_node* )mutex);
		  	 	m_free(pos);
		   		
//				print_string("delete mutex!\n");
		        ret = 1;
		   		break;
			}
		}
	}

	return ret;
}

void 
mutex_module_init(void)
{
    list_init(&gMutexlist);
}

void 
mutex_call_handler(uint cmd, uint param1, uint param2) 
{
    switch (cmd) {
    case 0x00:
	*(uint* )param1 = (uint)sys_mutex_create(param2);
	    break;

    case 0x01:
    sys_enter_critical((mutex_type* )param1, (uint* )param2);	
	    break;

	case 0x02:
    sys_exit_critical((mutex_type* )param1);	
	    break;

	case 0x03:
    *(int* )param2 = sys_destory_mutex((mutex_type* )param1);	
	    break;

	default:
	    break;
	}

}




#include "interrupt.h"
#include "task.h"
#include "mutex.h"
#include "screen.h"

extern task* p_task;

void sys_call_handler(uint call_number, uint cmd, uint param1, uint param2)
{
    switch (call_number) {
	case 0:
		task_sys_deal(cmd, param1, param2);
		break;
	case 1:
        mutex_call_handler(cmd, param1, param2);
		break;
    case 2:
        key_call_handler(cmd, param1, param2);
        break;

    default: 
	    break;
	}  
	
}

void segment_fault_handler()
{
	const char* err_info = p_task->name;
    
    set_print_pos(CMD_ERR_W, CMD_ERR_H);
	print_string("segment fault! kill task:");
	print_string(err_info);

	sys_call_handler(0, 0, 0, 0);
}


void page_fault_handler()
{
	const char* err_info = p_task->name;
 	static uint err_cnt = 0;

    set_print_pos(CMD_ERR_W, CMD_ERR_H);
	print_string("page fault! kill task:");
	print_string(err_info);
    
	sys_call_handler(0, 0, 0, 0);
//0xc5db
}

void timer_handler(void)
{
    static  int  i=0;
   
    // too short has some bugs...why
    i %=  4;
    if (i++ == 0x03) {
		schedule();
    }

    (*psend_EOI)(MASTER_EOI_PORT); 
}    


void keyboard_handler(void)
{
    uint sc = read_port(0x60);
    put_scan_code(sc);
    

    notify_keycode();
	(*psend_EOI)(MASTER_EOI_PORT); 

}    


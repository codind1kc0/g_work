#include "task.h"
#include "interrupt.h"
#include "screen.h"
#include "memory.h" 
#include "mutex.h"

#define   LEAVE   asm volatile("leave\n\r")
#define   IRET    asm volatile("iret\n\r")


// volatile
//void (* const pinit_Interrupt)(void) = NULL;
//void (* const penable_timer)(void) = NULL;

//save target task data struct!!!
uint index __attribute__((weak)) = 0;

static void  loading_UI(byte line)
{
    int i = 0;
	
	set_print_pos(0, line);
	print_char('[');
	set_print_pos(52, line);
	print_char(']');
	set_print_pos(2, line);

	for (i=0; i<49; i++) {
	    print_char('=');
        delay(1);
	} 

}

void KMain(void)
{
    void (* app_mod_init)(void) = (void* )BASE_OF_APP;
    
	print_string("OS starting ~~~ (kernel_version: v1.0.0 beta )");
//    loading_UI(1);
#if 0 
    uint temp = 0;
    
    print_string("GDT Entry: ");
    print_int_hex((uint)Ggdt_info.entry);
    print_char('\n');
    
    print_string("GDT Size: ");
    print_int_dec((uint)Ggdt_info.size);
    print_char('\n');
    
    print_string("IDT Entry: ");
    print_int_hex((uint)Iidt_info.entry);
    print_char('\n');
    
    print_string("IDT Size: ");
    print_int_dec((uint)Iidt_info.size);
    print_char('\n');
#endif
  
   memory_mod_init((byte*)KERNEL_HEAP_BASE, HEAP_SIZE);
   mutex_module_init();
   app_mod_init();

   task_mod_init();
   int_mod_init();
   keyboard_mod_init();
   
   config_page_table();
   launch_task();
   
}

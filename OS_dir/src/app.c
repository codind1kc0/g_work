#include "app.h"
#include "demo2.h"
#include "shell.h"
#include "sys_call.h"

#define  EASY_TASK(pre, add)     void  pre##add(void)

#if defined(EASY_TASK)
EASY_TASK(task_, A);
EASY_TASK(task_, B);
EASY_TASK(task_, C);
EASY_TASK(task_, D);
#endif

//void  app_mod_init(void)
void AppMain(void)
{
    //reg_app("shell", shell, 102);
    reg_app("taskA", task_A, 102);
//    reg_app("taskB", task_B, 102);
//    reg_app("taskC", task_C, 102);
    reg_app("taskD", task_D, 102);
    //reg_app("demo2", demo2_main, 232);
}


void task_A(void)
{
    int  j = 0;
    
	print_int_hex(task_D);
	*(int*)0x00 = 0;
    delay(2);
    set_print_pos(0, 6);
    print_string(__FUNCTION__);
   
	for (j=0; j<12; j++) {
        set_print_pos(10, 6);
	    print_char(j%26 + 'A');
	    delay(1);
	}
}

void task_B(void)
{
    int  k = 0;
    set_print_pos(0, 7);
    print_string(__FUNCTION__);

	for (k=0; k<1024; k++) {
        set_print_pos(10, 7);
	    print_char(k%26 + 'A');
	    delay(1);
	}

}

void task_C(void)
{   
    int  k = 0;
    set_print_pos(0, 8);
    print_string(__FUNCTION__);

    wait("taskA");
     
    set_print_pos(10, 8);
	print_string("waitting  over...\n");

	
}

void task_D(void)
{
    int  k = 0;

    set_print_pos(0, 9);
    print_string(__FUNCTION__);

    while (1) {
        set_print_pos(10, 9);
        print_char('a' + k++);
        
        k %= 26 ;
        delay(1);//shecdule in this.
    }

}

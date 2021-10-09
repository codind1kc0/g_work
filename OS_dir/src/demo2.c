#include "demo2.h"
#include "screen.h"

static  uint    g_write = 0;
static  uint    g_read  = 0;
static  uint    g_count = 0;
static  char    g_data  = 0;

static int  g_col  = (CMD_PINFO_H + 2);
static int  g_next = (CMD_PINFO_W); 
//static  const  NUM = 32; //default int
enum {
    W_NUM = 12,
};

static void  writer(void)
{
    int run = 0;
	while (1) {
	    enter_critical(g_write);
        
		++g_data;

	    exit_critical(g_write);

		if (run = (g_data < W_NUM) ) {
		    //continued ;
			set_print_pos(g_data % 30, CMD_PINFO_H);
			print_char(g_data % 26 + 'a');
		} else {
		    break;
		}
			delay(1);
	}

	enter_critical(g_write);
	g_data = 0;
    exit_critical(g_write);

}

static void  reader(void)
{
    int run = 1;

	while (run) {
	    enter_critical(g_read);
		if ( 0 == g_count) {
		    enter_critical(g_write);
		}

		++g_count;
		exit_critical(g_read);

		run = !!g_data;

		if (run) {
		    set_print_pos(g_next, g_col);
		    print_char(g_data%26 + 'a');
		}

        ++g_next;
        if (50 == g_next) {
		    g_next = 0;
			g_col++;
		}

		enter_critical(g_read);
		--g_count;
		if (0 == g_count) {
		    exit_critical(g_write);
		}
		exit_critical(g_read);

		delay(1);
	}
}

static void deinit_rw(void)
{ 
    delay(5);
    wait("writer");
	wait("reader1");
    wait("reader2");
    wait("reader3");

    destory_mutex(g_write);
    destory_mutex(g_read);
}


static void init_rw(void)
{
    g_write = create_mutex(nomal);
    g_read  = create_mutex(strict);

    g_col = CMD_PINFO_H + 2;
	g_next= 0;
	g_data = 0;
	g_count= 0;

	reg_app("writer",  writer, 222);
	reg_app("reader1", reader, 232);
	reg_app("reader2", reader, 232);
	reg_app("reader3", reader, 232);

}

static void demo_init(void)
{
    *(int*)0x0000 = 0x1111;
    init_rw();
	deinit_rw();
}

void demo2_main(void)
{
    reg_app("demo_init", demo_init, 122);
}


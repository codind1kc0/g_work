#include "shell.h"
#include "type.h"
#include "list.h"
#include "screen.h"
#include "demo2.h"

#define        MAX_KBUF_SIZE           0x100
static char    gKBuffer[MAX_KBUF_SIZE] = {0};
static uint    gKIndex = 0;
static t_list  gCmdList = {0};


enum {
    V_BACK  = 0x08, 
	V_ENTER = 0x0d,
};

typedef  struct _cmd_t{
    list_node   head_ndoe;
    const char* cmd_name;
    void  (*cmd_entry)();
}cmd_t;
// need a cmd list...

static void  backspace_handler(void)
{
    int del_pos = get_posw();

	if (del_pos > CMD_START_W) {
		set_print_pos(del_pos-1, CMD_START_H);
		print_char(' ');
		set_print_pos(del_pos-1, CMD_START_H);
    }

	gKIndex--;
}

static void  resert_cmd_line(void)
{
    int i = 0;
	set_print_pos(CMD_PRINT_W, CMD_START_H);

    while (i++ < POSW_MAX) {
	    print_char(' ');
	}
    set_print_pos(CMD_PRINT_W, CMD_START_H);
	print_string(CMD_STR);
	      
}

/**@@@***/
static void  add_cmd_entry(const char* cmd, void(*run)(void))
{
    cmd_t* rc = (cmd_t*)m_malloc(sizeof(cmd_t));

	if (rc && cmd && run) {
	    rc->cmd_name  = cmd;
		rc->cmd_entry = run;
		list_add(&gCmdList, (list_node*)rc);
	} else {
	    m_free(rc);
	}
}

static int   do_cmd(const char* cmds)
{
	int  ret = 0;
 
    if (cmds) {
    	list_node* pos = NULL;
		list_for_each(&gCmdList, pos) {
		    cmd_t* rc = (cmd_t*)pos;

			if ( str_cmp(cmds, rc->cmd_name, -1) ) {
			    rc->cmd_entry();
				ret = 1;
				break;
			}
		}      
	}

    return ret;
}


static void  undef_cmd(const char* str)
{
    int i = 0;
	set_print_pos(CMD_ERR_W, CMD_ERR_H);

    while (i++ < POSW_MAX) {
	    print_char(' ');
	}
	
	set_print_pos(CMD_ERR_W, CMD_ERR_H);
	print_string("Unknow Command: ");
	print_string(str);
}

static void  enter_handler(void)
{
    gKBuffer[gKIndex++] = '\0';

    /* skip none enter */
	if ( !do_cmd(gKBuffer) && (gKIndex > 1) ) {
	    undef_cmd(gKBuffer); 
	}

    gKIndex = 0;
	resert_cmd_line();
}

/* 
 * gKBuffer defend, but no good
 * to be: dynamic array...
 */
static void  push_buf(char ch)
{
    if ( MAX_KBUF_SIZE == gKIndex ) {
        gKIndex = 0;	
	}

	gKBuffer[gKIndex++] = ch;

}

static void  handler_key(char ch, byte vk)
{
    /* showing code. */
    if (ch) {
	    push_buf(ch);
        print_char(ch);	
	} else {
	    switch (vk) {
		case V_BACK:
		    backspace_handler();
		    break;

		case V_ENTER:
		    enter_handler();
		    break;

		default:break;
		}
	}
}

static uint  is_key_down(uint kc)
{
    return !!((0x01000000) & kc);
}

static inline char  get_key_char(uint kc)
{
    return (char)(kc & 0xff);
}

static inline byte  get_vkey_code(uint kc)
{
    return (byte)((kc & 0xff00)>>8); 
}

static void shell_clear(void)
{
    int w = 0;
	int h = 0;

    set_print_pos(CMD_PRINT_W, CMD_START_H);
	for (h=CMD_START_H; h<POSH_MAX; h++) {
	    for (w=0; w<POSW_MAX; w++) {
		    print_char(' ');
		}
	}
	
    set_print_pos(CMD_PRINT_W, CMD_START_H);
	print_string(CMD_STR);
    	
}

static void  do_wait(void)
{
    delay(5);
	wait("demo_init");
	resert_cmd_line();
}

static void  start_demo2(void)
{
    demo2_main();
    do_wait();
}

void  shell(void)
{
    list_init(&gCmdList);
	add_cmd_entry("clear", shell_clear);
// demo2 has some bug...
    add_cmd_entry("demo2", start_demo2);

	set_print_pos(CMD_PRINT_W, CMD_START_H);
    print_string(CMD_STR);
    
	while (1) {
	    uint key = read_key();
		if ( is_key_down(key) ){
	        char k_char  = get_key_char(key);
	        byte vk_code = get_vkey_code(key);
	        
			handler_key(k_char, vk_code);
		}
	}
}


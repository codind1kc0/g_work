#ifndef  KEYBOARD_H
#define  KEYBOARD_H

#include "utility.h"
#include "type.h"

#define KC_BUFF_SIZE    0x08

typedef struct kc_node_{
    uint buff[KC_BUFF_SIZE];
	uint head, tail;
	uint max;
	uint count;
} key_code_node;


void keyboard_mod_init(void);
/* scan code -> make code  */
void put_scan_code(byte sc);
/* fetch make code  */
uint fetch_key_code(void);
void notify_keycode(void);

/* sys_call deal  */
void key_call_handler(uint cmd, uint param1, uint param2);

#endif


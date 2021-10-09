#ifndef SCREEN_H
#define SCREEN_H

#include "type.h"
#include "const.h"

#define POSW_MAX  80
#define POSH_MAX  25

typedef enum {
    SCREEN_GRAY   = 0x07,
	SCREEN_BLUE   = 0x09,
	SCREEN_GREEN  = 0x0a,
	SCREEN_RED    = 0x0c,
	SCREEN_YELLOW = 0x0e,
	SCREEN_WHITE  = 0x0f,
}Print_color;

void clear_screen(void);
void set_print_pos(byte w, byte h);
void set_print_color(Print_color c);
byte print_char(char c);
int print_string(const char* s);
int print_int_dec(int n);
int print_int_hex(uint n);

byte get_posw(void);
byte get_posh(void);

#define        CMD_STR                 "Console $:"
#define        CMD_PRINT_W               0
#define        CMD_START_H               5
#define        CMD_START_W               str_len((const char* )CMD_STR)
#define        CMD_ERR_H                (CMD_START_H + 1)
#define        CMD_ERR_W                 0x00
#define        CMD_RTASK_H              (CMD_START_H + 3)
#define        CMD_RTASK_W               0X00

#define        CMD_PINFO_H              (CMD_START_H + 5)
#define        CMD_PINFO_W               0X00

#endif

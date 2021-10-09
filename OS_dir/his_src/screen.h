#ifndef SCREEN_H
#define SCREEN_H

#include "type.h"

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

#endif

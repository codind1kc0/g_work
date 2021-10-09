#include "global.h"

gdt_info  Ggdt_info = {0};
idt_info  Iidt_info = {0};

void (* const p_gfunc)(task* pt) = NULL;
void (*const pinit_Interrupt)(void) = NULL;
void (*const penable_timer)(void) = NULL;

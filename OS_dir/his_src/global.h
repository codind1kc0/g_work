#ifndef  GLOBAL_H
#define  GLOBAL_H

#include "kernel.h"
#include "const.h"

extern  gdt_info  Ggdt_info;
extern  idt_info  Iidt_info;
extern  void (* const p_gfunc)(task* pt);
extern  void (* const pinit_Interrupt)(void);
extern  void (* const penable_timer)(void);

#endif

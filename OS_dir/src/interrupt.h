#ifndef INTEERUPT_H
#define INTERRUPT_H

#include "kernel.h" 

typedef void (* p_void_func)(void);

extern  void (* volatile const pinit_Interrupt)(void);

extern  void (* volatile const penable_timer)(void);
extern  void (* volatile const psend_EOI)(uint port);
extern  uint  timer_handler_entry;
extern  uint  keyboard_handler_entry;
extern  uint  sys_call_handler_entry;
extern  uint  page_fault_handler_entry;
extern  uint  segment_fault_handler_entry;

void int_mod_init(void);
int set_Int_handler(gate* pgate, uint Ifunc);
int get_Int_handler(gate* pgate, uint* pIfunc);

#endif

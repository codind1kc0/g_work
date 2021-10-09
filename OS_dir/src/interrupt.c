#include "utility.h"
#include "interrupt.h"
#include "ihandler.h"


void (* volatile const pinit_Interrupt)(void) = NULL;
void (* volatile const psend_EOI)(uint port) = NULL;


void int_mod_init(void)
{
    set_Int_handler(AddressOff( Iidt_info.entry, 0x0d), (uint)&segment_fault_handler_entry);
    set_Int_handler(AddressOff( Iidt_info.entry, 0x0e), (uint)&page_fault_handler_entry);
    set_Int_handler(AddressOff( Iidt_info.entry, 0x20), (uint)&timer_handler_entry);
    set_Int_handler(AddressOff( Iidt_info.entry, 0x21), (uint)&keyboard_handler_entry);
    set_Int_handler(AddressOff( Iidt_info.entry, 0x80), (uint)&sys_call_handler_entry);

    (*pinit_Interrupt)();
}

int set_Int_handler(gate* pgate, uint Ifunc)
{
    int ret = 0;
    if (ret = (NULL != pgate)) {
        pgate -> offset1 = (Ifunc & 0xffff);
        pgate -> offset2 = ((Ifunc & 0xffff0000) >> 16);
        pgate -> selector = GDT_CFLAT32_SELECTOR;
        pgate -> dcount = 0; 
        pgate -> attr = (DA_386IGATE | DA_DPL3); //int in ring3

    }

    return ret;
}

int get_Int_handler(gate* pgate, uint* pIfunc)
{
    int ret = 0;
    if (ret = (pgate && pIfunc)) {
        *pIfunc = (pgate -> offset1 | ((pgate -> offset2)<<16));
    }
    
    return ret;
}
 


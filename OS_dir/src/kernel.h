#ifndef  KERNEL_H
#define  KERNEL_H

#include "type.h"
#include "const.h"

typedef struct descriptor_{
    ushort    limit1;
	ushort    base_addr_h1;
	byte      base_addr_h2;
	byte      attribute1;
	byte      attribute2_limit2;
	byte      base_addr_h3;

}descriptor;

typedef struct gdt_info_{
    descriptor*  const entry; // GDT start
	const int size;
}gdt_info;

typedef struct gate_ {
    ushort  offset1;
	ushort  selector;
	byte    dcount;
	byte    attr;
	ushort  offset2;
}gate;

typedef struct idt_info_ { 
    gate* const entry;
    const int size;
}idt_info;

// gdt's function
int set_descriptor_value(descriptor* pdesc, uint base, uint limit, uint attr);
int get_descriptor_value(descriptor* pdesc, uint* pbase, uint* plimit, uint* pattr);

// config page table
void config_page_table(void);

extern  gdt_info    Ggdt_info;
extern  idt_info    Iidt_info;

#endif

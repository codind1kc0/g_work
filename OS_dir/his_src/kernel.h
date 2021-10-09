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

typedef struct reg_value_{
    uint    gs;
	uint    fs;
    uint    es;
	uint    ds;

	uint    edi;
	uint    esi;
	uint    ebp;
	uint    kesp;
	uint    ebx;
	uint    edx;
	uint    ecx;
	uint    eax;

	uint    raddr;
	uint    eip;
	uint    cs;
	uint    eflags;
	uint    esp;
    uint    ss;

}reg_value;

typedef struct tss_ {
    uint    previous;
	uint    esp0;
	uint    ss0;
	
	uint    unused[22];
	ushort  reserved;
	ushort  iomap;
}TSS;

typedef struct task_{
    reg_value  rv; // 18
	descriptor ldt[3]; // 6
	TSS        tss; // 26 
	ushort     ldt_selector; // off:50*4
	ushort     tss_selector;
	uint       id;
	char       name[8];
	byte       stack[512];

}task;

// gdt's function
int set_descriptor_value(descriptor* pdesc, uint base, uint limit, uint attr);
int get_descriptor_value(descriptor* pdesc, uint* pbase, uint* plimit, uint* pattr);

// idt's function
int set_Int_handler(gate* pgate, uint Ifunc);
int get_Int_handler(gate* pgate, uint* pIfunc);

#endif

sys_address:
    base_of_boot        equ       0x7c00
	base_of_loader      equ       0x9000
	base_of_kernel      equ       0xb000

	base_of_shared_memery equ     0xa000

shared_value_adress:
    gdt_entry_addr       equ       (base_of_shared_memery + 0)
	gdt_size             equ       (base_of_shared_memery + 4)
    idt_entry_addr       equ       (base_of_shared_memery + 8)
	idt_size             equ       (base_of_shared_memery + 12)
	run_task_entry       equ       (base_of_shared_memery + 16)
	init_Interrupt_entry equ       (base_of_shared_memery + 20)
	enable_timer_entry   equ       (base_of_shared_memery + 24)

_8259a_data:
    MASTER_ICW1_PORT    equ        0x20
	MASTER_ICW2_PORT    equ        0x21
	MASTER_ICW3_PORT    equ        0x21
	MASTER_ICW4_PORT    equ        0x21

	SLAVE_ICW1_PORT    equ         0xa0
	SLAVE_ICW2_PORT    equ         0xa1
	SLAVE_ICW3_PORT    equ         0xa1
	SLAVE_ICW4_PORT    equ         0xa1

    MASTER_OCW1_PORT    equ        0x21
	MASTER_OCW2_PORT    equ        0x20
	MASTER_OCW3_PORT    equ        0x20

	SLAVE_OCW1_PORT    equ         0xa1
	SLAVE_OCW2_PORT    equ         0xa0
	SLAVE_OCW3_PORT    equ         0xa0

    MASTER_IMR_PORT    equ         0x21
	SLAVE_IMR_PORT     equ         0xa1


seg_property:
    DA_32      equ        0x4000
	DA_LIMIT_4K equ       0x8000
	DA_DR      equ        0x90
	DA_DRW     equ        0x92
	DA_DRWA    equ        0x93
	DA_C       equ        0x98
	DA_CR      equ        0x9a
	DA_CCO     equ        0x9c
	DA_CCOR    equ        0x9e

privilege_level:
    DA_DPL0    equ        0x00
	DA_DPL1    equ        0x20
	DA_DPL2    equ        0x40
	DA_DPL3    equ        0x60

gate_special_attribute:
    DA_TASKGATE    equ    0x85
	DA_386TSS      equ    0x89
	DA_386CGATE    equ    0x8c
    DA_386IGATE    equ    0x8e
	DA_386TGATE    equ    0x8f

_ldt_mode:
    DA_LDT     equ    0x82

selector_property:
    SA_RPL0    equ     0
    SA_RPL1    equ     1
    SA_RPL2    equ     2
    SA_RPL3    equ     3
    
	SA_TIG     equ     0
	SA_TIL     equ     4

page_attribute:
    PG_P    equ    1    ; 页存在属性位
    PG_RWR  equ    0    ; R/W 属性位值, 读/执行
    PG_RWW  equ    2    ; R/W 属性位值, 读/写/执行
    PG_USS  equ    0    ; U/S 属性位值, 系统级
    PG_USU  equ    4    ; U/S 属性位值, 用户级


descriptor_defined:
%macro Descriptor  3
    dw %2 & 0xffff
	dw %1 & 0xffff
    db (%1 >> 16) & 0xff
    dw (((%2 >> 8) & 0xf00) | (%3 & 0xf0ff))
    db (%1 >> 24) & 0xff
%endmacro

gate_defined:
%macro Gate 4
    dw %2 & 0xffff
	dw %1 & 0xffff
	dw (%3 & 0x1f) | (%4 << (3+5) )
	dw ( %2 >> 16 ) & 0xffff
%endmacro

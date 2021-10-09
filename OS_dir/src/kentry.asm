%include "common.asm"

global _start
global timer_handler_entry
global keyboard_handler_entry
global sys_call_handler_entry
global page_fault_handler_entry
global segment_fault_handler_entry

extern p_task

extern segment_fault_handler
extern page_fault_handler
extern timer_handler
extern sys_call_handler
extern keyboard_handler

extern pinit_Interrupt
extern psend_EOI
extern clear_screen
extern Ggdt_info
extern Iidt_info
extern p_gfunc
;extern pload_task
extern KMain

%macro  begin_FSR 0
    cli
	; fault auto push err_code.
    ; save g_reg
	pushad 
	push ds
	push es
	push fs
	push gs

    mov si, ss
	mov ds, si
	mov es, si

    ; esp re-point kernel stack
    mov esp, base_of_loader
%endmacro

%macro  end_FSR 0
	; restore esp
    mov esp, [p_task]

    pop gs
	pop fs
	pop es
	pop ds

    add esp, 4
    popad
%endmacro

%macro  begin_ISR 0
    ; don't think more interrupt 
	cli
    ; INT saved 5 registor!!!
	; skip err_code
    sub esp, 4
    ; save g_reg
	pushad 
	push ds
	push es
	push fs
	push gs

    mov si, ss
	mov ds, si
	mov es, si

    ; esp re-point kernel stack
    mov esp, base_of_loader
%endmacro

%macro  end_ISR 0
	; restore esp
    mov esp, [p_task]

    pop gs
	pop fs
	pop es
	pop ds
    popad
	; error code must skip
    add esp, 4
%endmacro


[section .text]
[bits 32]
_start:
;in kernel first : 
;1. init global vals 
	call init_global
	call clear_screen
    call KMain

ret_next:
	jmp $

;
;init global for C file
;must use stdcall
init_global:
    push ebp  
    mov ebp, esp
    
    mov eax, dword [gdt_entry_addr]
	mov dword [Ggdt_info], eax
    mov eax, dword [gdt_size]
	mov dword [Ggdt_info + 4], eax

    mov eax, dword [idt_entry_addr]
	mov dword [Iidt_info], eax
    mov eax, dword [idt_size]
	mov dword [Iidt_info + 4], eax

	mov eax, dword [run_task_entry]
	mov dword [p_gfunc], eax

	mov eax, dword [init_Interrupt_entry]
	mov dword [pinit_Interrupt], eax
	
	mov eax, dword [send_EOI_entry]
	mov dword [psend_EOI], eax

;	mov eax, dword [load_task_entry]
;	mov dword [pload_task], eax
; leave induction
    mov esp, ebp
	pop ebp
    ret

; ISR entry
timer_handler_entry:
begin_ISR
	call timer_handler
end_ISR
    iret
	; esp0 pointor reg_valuei's bottom.

keyboard_handler_entry:
begin_ISR
	call keyboard_handler
end_ISR
    iret
	
sys_call_handler_entry:
;use c_decl
begin_ISR
;some paramer
    push edx
	push ecx
	push ebx
	push eax
	call sys_call_handler
	pop  eax
	pop  ebx
	pop  ecx
	pop  edx
end_ISR
    iret

page_fault_handler_entry:
begin_FSR
    call page_fault_handler
end_FSR
    iret

segment_fault_handler_entry:
begin_FSR
    call segment_fault_handler
end_FSR
    iret

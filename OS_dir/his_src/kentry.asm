%include "common.asm"

global _start

extern pinit_Interrupt
extern penable_timer
extern clear_screen
extern Ggdt_info
extern Iidt_info
extern p_gfunc
extern KMain

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
	
	mov eax, dword [enable_timer_entry]
	mov dword [penable_timer], eax
	

; leave induction
    mov esp, ebp
	pop ebp
    ret

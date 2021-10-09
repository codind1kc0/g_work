%include "common.asm"

global _start
global app_mod_init

extern AppMain
extern memory_mod_init

[section .text]
[bits 32]
_start:
app_mod_init:  ;0xf000? 0x10000?
    push ebp
	mov ebp, esp

    mov dword [app_main_entry], AppMain
    
	push  heap_size
	push  app_heap_base
	call  memory_mod_init
	add   esp, 8

    
	leave
	ret



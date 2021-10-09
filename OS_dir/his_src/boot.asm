%include "blfunc.asm"
%include "common.asm"

org base_of_boot
; interface
stack_base      equ        base_of_boot 
base_of_target  equ        base_of_loader
target_str      db         "LOADER     "
target_len      equ        ($ - target_str)

;boot entry
bl_main:
   mov ax, cs
   mov ds, ax
   mov es, ax
   mov ss, ax
   mov sp, sp_init_value

   call loader_target
   cmp dx, 0
   jne  err_boot
   jmp base_of_loader

err_boot:
   mov bp, err_load
   mov cx, err_len
   call print
   jmp $ 

err_load  db    "no loader!"
err_len   equ   ($ - err_load)
buf:
	times 510-($-$$) db 0x00
	db 0x55, 0xaa

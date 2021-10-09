.section .text
.code16
.globl _start
_start:
    movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
    movw $str_data, %bp
	movw str_len, %cx
	call print
   
end_boot:
    hlt
	jmp end_boot


//ES:BP is argument of string
//CX    is argument of str_lenth
print:
    movw $0x1301, %ax
	movw $0x0001, %bx
	int $0x10
    ret

str_data:
    .asciz "Hello OS."
str_len:
    .byte 9 
.org 510
end_:
    .byte 0x55, 0xaa


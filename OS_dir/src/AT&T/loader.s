.code16
.section text
info_str:
.asciz "hello wold!\n"
.byte 0x00

movw $0x00, %ax
int $0x10

movb $0x03, %ah
int $0x10

movw $12, %cx
movw $0x0005, %bx

movw $info_str, %bp
movw $0x1301, %ax
int $0x10

last:
    hlt 
jmp last

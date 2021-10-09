;function name delay
delay:
    %rep 5
    nop	
	%endrep
    ret

;name init_8259a
init_8259a:
; ICW1-master
    mov al, 0x11
	out 0x20, al
	call delay

; ICW2-master
    mov al, 0x20
	out 0x21, al
	call delay
    
; ICW3-master
    mov al, 0x04
	out 0x21, al
	call delay

; ICW4-master
    mov al, 0x11
	out 0x21, al
	call delay
	
; ICW1-slaver
    mov al, 0x11
	out 0xa0, al
	call delay

; ICW2-slaver
    mov al, 0x28
	out 0xa1, al
	call delay
    
; ICW3-slaver
    mov al, 0x02
	out 0xa1, al
	call delay

; ICW4-slaver
    mov al, 0x01
	out 0xa1, al
	call delay
	
	ret

;name read_IMR
read_master_IMR:
    in ax, 0x21
	call delay
    ret

read_slaver_IMR:
    in ax, 0xa1
	call delay
    ret

;name write_IMR
; mask in al
write_master_IMR:
    out 0x21, al
	call delay
    ret

write_slaver_IMR:
    out 0xa1, al
	call delay
    ret

;name write_EOI
;port in dx
write_EOI:
    push ax
    mov al, 0x20
	out dx, al
    call delay
    pop ax
	ret

write_master_EOI:
    and al, 0x07
	or al, 0xe0
	out 0x20, al
	call delay
    ret

wriie_slaver_EOI:
    and al, 0x07
	or al, 0xe0
	out 0xa0, al
	call delay
    ret

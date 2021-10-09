org 0x7c00

;BS_JmpBoot
jmp short start
nop 

sys_define:
    stack_base equ     0x7c00
    base_of_loader equ 0x9000
     
	fat_table_addr equ 0x7e00

    root_dir_sector equ 19
	root_dir_len    equ 14
	entry_item_len  equ 32
	fat_entry_len   equ 9

	fat_entry_offset equ 1; 1 or 10
;FAT12 file system head
header:
    BS_OEMName     db  "kOS_learn"
	BPB_BytsPerSec dw  0x200
	BPB_SecPerClus db  1
	BPB_RsvdSecCnt dw  1
	BPB_NumFATs    db  2
	BPB_RootEntCnt dw  0xe0  ;224
	BPB_TotSec16   dw  0xb40 ;2880
	BPB_Media      db  0xF0  
	BPB_FATSz16    dw  9
	BPB_SecPerTrk  dw  18
	BPB_NumHeads   dw  2
	BPB_HiddSec    dd  0
	BPB_TotSec32   dd  0
	BS_DrvNum      db  0  ;floopy number
	BS_Reserved1   db  0
	BS_BootSig     db  0x29
	BS_VolID       dd  0
	BS_VolLab      db  "new_001    "
	BS_FileSysType db  "FAT12   "

start:
    mov ax, cs
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov sp, stack_base

    mov ax, root_dir_sector
    mov cx, root_dir_len
	mov bx, buf
	call read_sector	

    mov si, test_msg
	mov cx, 11
    call found_root_dir

    cmp bx, 0xffff
    je last
    
	; copy entry_item
	mov di, entry_item 
	mov si, bx 
	mov cx, entry_item_len 
    call mem_copy

    ; copy fat1 table to 0x9000
    mov ax, fat_entry_len
	mov bx, [BPB_BytsPerSec]
	mul bx ; 512*9
    mov bx, base_of_loader
	sub bx, ax

	mov ax, fat_entry_offset
	mov cx, fat_entry_len
	call read_sector

;	mov cx, [entry_item + 0x1a] ; start cluster number(in entry_item).
;	call fat_vec_read ; vec[values]
    mov bx, fat_table_addr ;;;;;
	;mov bp, entry_item
	mov dx, [entry_item+0x1a] ; start cluster

    mov si, base_of_loader

copy_loader:
    push dx
    push bx

    add dx, 0x1f ; logic sector number
	cmp dx, 0xff7
    jae  base_of_loader ; control change 0x9000

    mov ax, dx
	mov cx, 1
    mov bx, si
	call read_sector

    pop bx
	pop cx 
	call fat_vec_read
    
	add si, 0x200 ; next sector read

	jmp copy_loader


;end_l:
;    call debug
last:
    hlt
	jmp last

;;;;;;;;;;;;;;;;;;;;;;;;;;         ;;;;;;;;;;;;;;;;;;;;             
;debug:                  ;;         ;;                        
;   ;push bp             ;;         ;;                        
;   ;push cx             ;;         ;;                       
;                        ;;         ;;                      
;   mov bp, base_of_loader          ;;         ;;                     
;   mov cx, 0x1800       ;;         ;;;;;;;;;;;;;;;;;;    
;   call print          ;;         ;;                   
;                        ;;         ;;
;   ;pop cx              ;;         ;;                        
;   ;pop bp              ;;         ;;
;	ret                 ;;         ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;         ;;;;;;;;;;;;;;;;;;;    TOOLS   

;======================== function ========================== ;
;============================================================ ;

;function name: fat_vec_read
;             index is : CX
; fat table address is : BX
; return fat_vec[index] value in DX
fat_vec_read:
    push ax
	push bx
	push cx
	push si

	mov ax, cx
	mov cl, 2
    div cl 
	cmp ah, 1
	je _odd_vec

_even_vec:
    mov cl, 3
    mul cl
	mov si, ax
	mov dx, word [bx+si]
    and dx, 0x0fff
    jmp fat_vec_end

_odd_vec:
    mov cl, 3
    mul cl
	mov si, ax
	mov dx, word [bx+si+1]
    shr dx, 4

fat_vec_end:
	pop si
	pop cx
	pop bx
	pop ax
    ret


;function name: mem_copy
;       source in: DS:SI
;  destination in: ES:DI
;copy number(/byte) in CX
mem_copy:
    push ax
	push bx
	push cx
	push si
	push di

	cmp cx, 0
	je end_

	cmp si, di
    jl m_etob_copy
m_btoe_copy:
    mov al, byte [si]
	mov byte [di], al
	inc si
	inc di
    loop m_btoe_copy    
    jmp end_

m_etob_copy:
    mov bx, cx
	mov al, byte [si+bx-1]
	mov byte [di+bx-1], al
	loop m_etob_copy

end_:
    pop di
	pop si
    pop cx
    pop bx
    pop ax
	ret


;function name: found_root_dir
;    found number in ES:BX(A/32)
;    found number in DX(MAX )
;    globl name   in DS:SI
;    name string lenth in CX
;if not found, BX values equal 0xffff; other, BX meaning offset 
found_root_dir:
    push cx
    push di
	push dx
    push bp

    mov dx, 0xe0
	mov bp, sp
found_loop:  
    mov cx, [bp+6]
    mov di, bx
	call mem_compare
	jcxz return_ok

    add bx, 0x20
    dec dx
    jnz found_loop
    mov bx, 0xffff ; no't found name string

return_ok:
    
	pop bp
	pop dx
	pop di
	pop cx
	ret

;function name: mem_compare: 
;           src in DS:SI
;		    dest  in ES:DI
;		    count in CX
;return value: if cx == 0: success
mem_compare:
    push ax
    push si
	push di

	cmp cx,0
	jle stop

compare:
    mov al, byte [di]
    cmp al, byte [si]
    jnz stop
	
next:
    inc si
    inc di
    loop compare
stop:
    
	pop di 
    pop si
	pop ax
    ret

;reset floopy; no paramer
reset_floppy:
	push ax
	push dx
    
	mov ah, 0x00
	mov dl, [BS_DrvNum]
	int 0x13

    pop dx
	pop ax
	ret

; function name: read_sector
; read floopy's sector
;logic sector number in   AX
;target address in     ES:BX
;read sector number  in   CX
read_sector:
    push bx
    push cx
    push dx
    push ax
     
    call reset_floppy					      
    push bx         ;protect bx
    push cx         ;too...
      
    mov bl, [BPB_SecPerTrk]
    div bl
    mov cl, ah
    add cl, 1
    mov ch, al
    shr ch, 1
    mov dh, al
    and dh, 1
    mov dl, [BS_DrvNum]
	      
    pop ax
    pop bx
																						      
    mov ah, 0x02

read:    
    int 0x13
    jc read
																										      
    pop ax
    pop dx
    pop cx
    pop bx
																												      
    ret


;ES:BP is argument of string
;CX    is argument of str_lenth
print:
    mov dx, 0x0000 ;(0, 0)
    mov ax, 0x1301
	mov bx, 0x0007
    int 0x10
	ret

test_msg:
    db "TEST1      "

msg_str:
	db "Hello, OS!"
msg_len equ  ($-msg_str)

entry_item: 
    times entry_item_len db 0

buf:
	times 510-($-$$) db 0x00
	db 0x55, 0xaa

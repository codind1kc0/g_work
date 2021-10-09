%include "blfunc.asm"
%include "common.asm"

org base_of_loader

stack_base      equ    base_of_loader
;base_of_target  equ    base_of_kernel 
kernel_str      db     "KERNEL     "
kernel_len      equ    ($ - kernel_str)
app_str      db     "APP        "
app_len      equ    ($ - app_str)

[section .idt]
align 32
[bits 32]
IDT_ENTRY:
%rep 256 
    Gate  code32_selector, def_handle, 0, (DA_386IGATE | DA_DPL3)
%endrep

IDT_LEN  equ    $-IDT_ENTRY

IDT_PTR:
    dw  IDT_LEN-1
    dd  0


; [section .gdt] start
[section .gdt]
; GDT_TAB                       start_addr     limit_addr         seg_property
GDT_ENTRY:         Descriptor      0,             0,                    0
CODE32_DESC:       Descriptor      0,         code32_len - 1,      (DA_C | DA_32 | DA_DPL0)
CODE32_FLAT_DESC:  Descriptor      0,           0xfffff,           (DA_C | DA_32 | DA_DPL0)
VIDEO_DESC:        Descriptor    0xb8000,        0x7fff,           (DA_DRWA | DA_32 | DA_DPL0)     
DATA32_DESC:       Descriptor      0,         data32_len - 1,      (DA_DRW | DA_32 | DA_DPL0)     
DATA32_FLAT_DESC:  Descriptor      0,           0xfffff,           (DA_DRW | DA_32 | DA_DPL0)
TASK_LDT_DESC:     Descriptor      0,                0,                 0 
TASK_TSS_DESC:     Descriptor      0,                0,                 0
PAGE_DIR_DESC:     Descriptor      page_dir_base,   4095,          (DA_DRW | DA_32 | DA_DPL0)
PAGE_TBL_DESC:     Descriptor      page_tbl_base,   1023,          (DA_DRW | DA_LIMIT_4K | DA_32 | DA_DPL0)

GDT_LEN   equ      $ - GDT_ENTRY


GDT_PTR: 
    dw GDT_LEN-1    ; gdt_max_offset
    dd  0           ; gdt_addr 


GDT_SELECTOR:       ; first not use.
    code32_selector         equ    ( (0x0001 << 3) | SA_TIG | SA_RPL0 )
    code32_flat_selector    equ    ( (0x0002 << 3) | SA_TIG | SA_RPL0 )
    video_selector          equ    ( (0x0003 << 3) | SA_TIG | SA_RPL0 )
    data32_selector         equ    ( (0x0004 << 3) | SA_TIG | SA_RPL0 )
    data32_flat_selector    equ    ( (0x0005 << 3) | SA_TIG | SA_RPL0 )
    gfunc_selector          equ    ( (0x0006 << 3) | SA_TIG | SA_RPL0 )
	page_dir_selector       equ    ( (0x0008 << 3) | SA_TIG | SA_RPL0 )
	page_tbl_selector       equ    ( (0x0009 << 3) | SA_TIG | SA_RPL0 )
;end of [section .gdt]


[section .s16]
[bits 16]
bl_main:
mov ax, cs
mov ds, ax
mov es, ax
mov ss, ax

mov sp, sp_init_value

; loade app 
push word  buf
push word  base_of_app / 0x10
push word  base_of_app
push word  app_len
push word  app_str
call loader_target
add sp, 10
cmp dx, 0
jne app_err_deal

; loade kernel
push word  buf
push word  base_of_kernel / 0x10
push word  base_of_kernel
push word  kernel_len
push word  kernel_str
call loader_target
add sp, 10
cmp dx, 0
jne kernel_err_deal

mov esi, code32_seg_start
mov edi, CODE32_DESC
call init_descriptor_item

mov esi, data32_seg_start
mov edi, DATA32_DESC
call init_descriptor_item

;init gdt struct
xor eax, eax
mov ax, ds
shl ax, 4
add eax, GDT_ENTRY
mov dword [GDT_PTR + 2], eax

;init idt struct
xor eax, eax
mov ax, ds
shl ax, 4
add eax, IDT_ENTRY
mov dword [IDT_PTR + 2], eax

;load gdt table info in 0xa000
call store_global
; load gdt
lgdt [GDT_PTR]

; load idt
cli 
lidt [IDT_PTR]

; set IOPT == 3
pushf

pop eax
or eax, 0x3000
push eax

popf

; open A20
in al, 0x92
or al, 0x02
out 0x92, al
;protect open
mov eax, cr0
or eax, 0x01
mov cr0, eax

;jmp dword code32_selector : 0
;push stack32_selector
;push stack32_limit
push code32_selector
push 0x0000
retf

app_err_str     db    "no app module."
app_err_len     equ    ($ - app_err_str)

kernel_err_str  db    "no kernel module." 
kernel_err_len  equ   ($ - kernel_err_str)


app_err_deal:
    mov bp, app_err_str
    mov cx, app_err_len
	jmp print_err
kernel_err_deal:
    mov bp, kernel_err_str
    mov cx, kernel_err_len
print_err:
    mov ax, cs
	mov es, ax
    call print
    jmp $

; name: init_descriptor_item
; code seg label   -> ESI
; descriptor label -> EDI
init_descriptor_item:
    push eax

    xor eax, eax
    mov ax, cs
    shl eax, 4

    add eax, esi
    mov word [edi + 2], ax
    shr eax, 16 ;;;;;;;;;;;;;;;;;;;;;;E1
    mov byte [edi + 4], al
    ;shr eax, 8
    mov byte [edi + 7], ah

    pop eax
    ret
      
; store data to shared memery      
store_global:
    mov dword [run_task_entry], run_task
;    mov dword [load_task_entry], load_task
    mov dword [init_Interrupt_entry], init_Interrupt
    mov dword [enable_timer_entry], enable_timer
    mov dword [send_EOI_entry], send_EOI

    mov eax, dword [GDT_PTR + 2]
    mov dword [gdt_entry_addr], eax
    mov dword [gdt_size], (GDT_LEN / 8) 

    mov eax, dword [IDT_PTR + 2]
    mov dword [idt_entry_addr], eax
    mov dword [idt_size], (IDT_LEN / 8) 
    
; more info
    ret

[section .isrfunc]
[bits 32]
; ======== 8259a init code and some int setting function 
Init8259A:
    push ax
        
    ; master
    ; ICW1
    mov al, 00010001B
    out MASTER_ICW1_PORT, al
                        
    call delay
                                    
    ; ICW2
    mov al, 0x20
    out MASTER_ICW2_PORT, al
                                                
    call delay
                                                        
    ; ICW3
    mov al, 00000100B
    out MASTER_ICW3_PORT, al
                                                                        
    call delay
                                                                        
    ; ICW4
    mov al, 00010001B
    out MASTER_ICW4_PORT, al
    call delay
    ; slave
    ; ICW1
    
    mov al, 00010001B
    out SLAVE_ICW1_PORT, al
                                                                                                                                
    call delay
                                                                                                                                        
    ; ICW2
    mov al, 0x28
    out SLAVE_ICW2_PORT, al
                                                                                                                                                        
    call delay
                                                                                    
    ; ICW3        
    mov al, 00000010B
    out SLAVE_ICW3_PORT, al
    call delay
                                                                            
    ; ICW
    mov al, 00000001B
    mov al, 00000001B
    out SLAVE_ICW4_PORT, al
    call delay
    
    pop ax
    ret

;name delay
delay:
    %rep 5
    nop
    %endrep
    ret
    
;write_IMR
write_IMR:
    push ax
    push dx
 
    out dx, al
    call delay

    pop dx
    pop ax
    ret
    
;read_IMR
read_IMR:
    push dx
    in al, dx
    call delay
    pop dx
    ret

;OCW2 port in dx
write_EOI:
    push ax
    ;clear interrupt by self
    mov al, 0x20
    out dx, al
    call delay
    pop ax
    ret

; kernel globl function seg
[section .gfunc]
[bits 32]
; name: run_task 
; paramter: task* pt, not return value
run_task:
    push ebp
    mov ebp, esp

    mov esp, [ebp + 8] ; esp -> &(pt->rv.gs)
    lldt word [esp + 96] 
    ltr word [esp + 98]

    pop gs ; pop all of registor
    pop fs
    pop es
    pop ds

    popad

    add esp, 4 ;temp deal with skip raddr

;   open timer.
    mov dx, MASTER_IMR_PORT
	in  al, dx
%rep 5
    nop
%endrep

    and al, 0xfc
    out dx, al
%rep 5
    nop
%endrep

    mov eax, cr0
	or  eax, 0x80000000
    mov cr0, eax
    iret

;name load_task
;void load_task(task* p);
load_task:
    push ebp
    mov ebp, esp

    mov esp, [ebp + 8]
    lldt word [esp + 96]
    
    leave
    ret
     

;name: init_Interrupt
init_Interrupt:
    push ebp
    mov ebp, esp

    push ax
    push dx
 
    call Init8259A
 
 ; open cpu interrupt flag
    sti

    mov ax, 0xff
    mov dx, MASTER_IMR_PORT
    call write_IMR

    mov ax, 0xff
    mov dx, SLAVE_IMR_PORT
    call write_IMR
 
    pop dx
    pop ax

    leave 
    ret

; name: enable_timer
; just open timer
enable_timer:
    push ebp
    mov ebp, esp
    push ax
    push dx
 
    mov dx, MASTER_IMR_PORT
    call read_IMR
    and al, 0xfe
;error:2021-3-17 forget open master IR0
    call write_IMR
 
    pop dx
    pop ax

    leave 
    ret

;name: send_EOI
;void send_EOI(uint port)
; port ==> 8259A port
;0x9c5b
send_EOI:
    push ebp
    mov ebp, esp
    
    mov edx, dword [ebp + 8]
    mov al, 0x20
    out dx, al
    
    ; make sure send success
    call delay
    
    leave
    ret


;******************************************** 32 ********************************************;
[section .s32]
[bits 32]
code32_seg_start:

    mov ax, video_selector
    mov gs, ax

    mov ax, data32_flat_selector
    mov ds, ax
    mov es, ax
    mov fs, ax


    mov ss, ax
    mov esp, base_of_loader 

    xor eax, eax

    call init_page_table

    jmp dword code32_flat_selector : base_of_kernel

;
; default interrupt handle function.
def_handle_func:
    iret
def_handle     equ         def_handle_func - $$

;name: set_up_page
; page table director selector => eax
; page table selector          => ebx
; page table base              => ecx
init_page_table:
    push edi
	push es
	push ecx
	push ebx
	push eax

    mov ax, page_dir_selector; ax
	mov es,  ax
	xor edi, edi
    
	mov eax, page_tbl_base ;[esp + 8]
	or  eax, PG_P | PG_USU | PG_RWW
	mov ecx, 0x400
	cld

page_dir_init:
	stosd
    add eax, 0x1000
    loop page_dir_init

    mov ax, page_tbl_selector ;word [esp + 4]
	mov es, ax
	xor edi, edi
	mov eax, PG_P | PG_USU | PG_RWW
	mov ecx, 0x100000
	cld

page_tbl_init:
    stosd
	add eax, 0x1000
	loop page_tbl_init

end_page_init: 
; error : 2021/07/15. mov eax, page_dir_selector
; result: open page mode error
    mov eax, page_dir_base
    mov cr3, eax

	pop eax
	pop ebx
	pop ecx
	pop es
	pop edi
    ret

code32_len  equ    ($ - code32_seg_start)

[section .data32]
[bits 32]
data32_seg_start:
    times 2048 db 0
data32_len equ $-data32_seg_start

buf db 0

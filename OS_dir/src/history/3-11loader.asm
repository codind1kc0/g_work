%include "inc.asm"

init_param:
    stack_top       equ    0x7c00
	page_dir_base0  equ    0x200000
	page_tbl_base0  equ    0x201000
	page_dir_base1  equ    0x601000
	page_tbl_base1  equ    0x602000


org 0x9000
jmp entry_seg 
; [section .gdt] start
[section .gdt]
; GDT_TAB                  start_addr     limit_addr         seg_property
GDT_ENTRY:    Descriptor      0,             0,                    0
CODE32_DESC:  Descriptor      0,         code32_len - 1,      DA_C | DA_32 | DA_DPL0
VIDEO_DESC:   Descriptor   0xB8000,       0x07FFF,            DA_DRWA | DA_32 | DA_DPL0
DATA32_DESC:  Descriptor      0,         data32_len-1,        DA_DRW  | DA_32 | DA_DPL0
STACK32_DESC: Descriptor      0,         stack32_limit,       DA_DRW | DA_32 | DA_DPL0
CODE16_DESC:  Descriptor      0,           0xFFFF,            DA_C | DA_DPL0 
UPDATE_DESC:  Descriptor      0,           0xFFFF,            DA_DRW | DA_DPL0
A_LDT_DESC:   Descriptor      0,        taska_ldt_len - 1,    DA_LDT | DA_DPL0       

FUNCTION_DESC:Descriptor      0,        function_len - 1,     DA_C | DA_32 | DA_DPL0

;Gate_descriptor
;call gate
;                                selector             offset            param_count          attribute
FUNC_CG_PRINT_DESC:     Gate   function_selector,     print_info_off,        0,             ( DA_386CGATE | DA_DPL3 )
;ACCE_KERNEL_DATA_DESC: Gate   function_selector,     acce_kernel_off,       0,             ( DA_386CGATE | DA_DPL3 )
INIT_PAGE_TBL_DESC      Gate   function_selector,     page_tbl_init_off,     0,             ( DA_386CGATE | DA_DPL3 )
SWITCH_PAGE_TBL_DESC    Gate   function_selector,     page_tbl_switch_off,   0,             ( DA_386CGATE | DA_DPL3 ) 

;Tss_descriptor
;tss
TSS_DESC:     Descriptor      0,         tss_len - 1,         DA_386TSS | DA_32 | DA_DPL0

;page table descriptor
PAGE_DIR_DESC0 Descriptor    page_dir_base0,   4095,           DA_DRW | DA_32 | DA_DPL0
PAGE_TBL_DESC0 Descriptor    page_tbl_base0,   1023,           DA_DRW | DA_LIMIT_4K | DA_32 | DA_DPL0

PAGE_DIR_DESC1 Descriptor    page_dir_base1,   4095,           DA_DRW | DA_32 | DA_DPL0
PAGE_TBL_DESC1 Descriptor    page_tbl_base1,   1023,           DA_DRW | DA_LIMIT_4K | DA_32 | DA_DPL0

;flat mode descriptor
FLAT_MODE_DESC Descriptor    0,         0xfffff,              DA_DRW | DA_LIMIT_4K |DA_32 | DA_DPL0

; test code data descriptor
RDATA_DESC     Descriptor    0,         rdata_len - 1,        DA_DR | DA_32 | DA_DPL0
FLAT_CODE_DESC Descriptor    0,         0xfffff,              DA_C  | DA_LIMIT_4K | DA_32 | DA_DPL0

; !!!!!!!! system info in here
SYS_DATA_DESC  Descriptor    0,         sys_info_len - 1,     DA_DR  | DA_32 | DA_DPL0
GDT_LEN   equ      $ - GDT_ENTRY

;===========================================================================================================GDT end;

GDT_PTR: 
    dw GDT_LEN-1    ; gdt_max_offset
	dd  0           ; gdt_addr 



GDT_SELECTOR:       ; first not use.
    code32_selector    equ    ( (0x0001 << 3) | SA_TIG | SA_RPL0 )
    video_selector     equ    ( (0x0002 << 3) | SA_TIG | SA_RPL0 )
	data_selector      equ    ( (0x0003 << 3) | SA_TIG | SA_RPL0 )
	stack32_selector   equ    ( (0x0004 << 3) | SA_TIG | SA_RPL0 )
	code16_selector    equ    ( (0x0005 << 3) | SA_TIG | SA_RPL0 )
	update_selector    equ    ( (0x0006 << 3) | SA_TIG | SA_RPL0 )
	taska_ldt_selector equ    ( (0x0007 << 3) | SA_TIG | SA_RPL0 )
	function_selector  equ    ( (0x0008 << 3) | SA_TIG | SA_RPL0 )

;Gate selector defined: call gate
    func_print_selector  equ    ( (0x0009 << 3) | SA_TIG | SA_RPL3 )
    ;func_acce_selector   equ    ( (0x000a << 3) | SA_TIG | SA_RPL3 )
    init_page_tbl_selector equ  ( (0x000a << 3) | SA_TIG | SA_RPL3 )
    switch_page_tbl_selector equ ( (0x000b << 3) | SA_TIG | SA_RPL3 )
;tss selector
    tss_selector       equ     ( (0x000c << 3) | SA_TIG | SA_RPL0 )

;page table selector
    page_dir_selector0  equ    ( (0x000d << 3) | SA_TIG | SA_RPL0 )
	page_tbl_selector0  equ    ( (0x000e << 3) | SA_TIG | SA_RPL0 )
	page_dir_selector1  equ    ( (0x000f << 3) | SA_TIG | SA_RPL0 )
	page_tbl_selector1  equ    ( (0x0010 << 3) | SA_TIG | SA_RPL0 )
    
;flat mode selector
    flat_mode_selector  equ    ( (0x0011 << 3) | SA_TIG | SA_RPL0 )

; rdata selector
    rdata_selector      equ    ( (0x0012 << 3) | SA_TIG | SA_RPL0 )
flat_code_selector      equ    ( (0x0013 << 3) | SA_TIG | SA_RPL0 )

sys_info_selector       equ    ( (0x0014 << 3) | SA_TIG | SA_RPL0 )
;end of [section .gdt]

;&&& ldt &&&
[section .task_a_ldt]
taska_ldt_start:
;LDT_TAB                              start_addr        limit_addr            seg_property
TASKA_CODE32_DESC:  Descriptor             0,       taska_code32_len-1,       DA_C | DA_32 | DA_DPL3
TASKA_DATA32_DESC:  Descriptor             0,       taska_data32_len-1,       DA_DRW | DA_32 | DA_DPL3
TASKA_STACK32_DESC: Descriptor             0,       taska_stack32_len-1,      DA_DRW | DA_32 | DA_DPL3

taska_ldt_len    equ      $-taska_ldt_start

;LDT end

TASKA_LDT_SELECTOR:
    taska_code32_selector     equ        ( (0x0000<<3) | SA_TIL | SA_RPL3 )
    taska_data32_selector     equ        ( (0x0001<<3) | SA_TIL | SA_RPL3 ) ;RPL3 can't access kernel data
    taska_stack32_selector    equ        ( (0x0002<<3) | SA_TIL | SA_RPL3 )

; SOME SYSTEM INFOMATION 
[section .sys_data]
sys_data_start:
; data of mem info
MEM_SIZE:
    times  4  db 0      
mem_size_off equ   MEM_SIZE-$$

MEM_ARDS_COUNT:
    times 4 db 0
ards_count_off equ   MEM_ARDS_COUNT-$$

ARDS_ARR:
    times  20*64  db 0
ards_off equ   ARDS_ARR-$$

sys_info_len  equ    $ - sys_data_start

[section .s16]
[bits 16]
entry_seg:

mov ax, cs
mov ds, ax
mov es, ax
mov ss, ax

mov [ret_to_real + 3], ax

mov sp, stack_top

nop
nop
call init_memory_info
nop
nop
; init GDT for 32bits seg

mov esi, sys_data_start
mov edi, SYS_DATA_DESC
call init_descriptor_item

mov esi, code32_seg_start
mov edi, CODE32_DESC
call init_descriptor_item

mov esi, stack32_start
mov edi, STACK32_DESC
call init_descriptor_item

mov esi, data32_start
mov edi, DATA32_DESC
call init_descriptor_item

mov esi, code16_start
mov edi, CODE16_DESC
call init_descriptor_item

mov esi, taska_ldt_start
mov edi, A_LDT_DESC
call init_descriptor_item

mov esi, taska_code_start
mov edi, TASKA_CODE32_DESC
call init_descriptor_item

mov esi, taska_data_start
mov edi, TASKA_DATA32_DESC
call init_descriptor_item

mov esi, taska_stack_start
mov edi, TASKA_STACK32_DESC
call init_descriptor_item

mov esi, function_seg_start
mov edi, FUNCTION_DESC
call init_descriptor_item

mov esi, tss_start
mov edi, TSS_DESC
call init_descriptor_item

mov esi, rdata_start
mov edi, RDATA_DESC
call init_descriptor_item

; init gdt struct
xor eax, eax
mov ax, ds
add eax, GDT_ENTRY
mov dword [GDT_PTR + 2], eax

;1. load gdt
lgdt [GDT_PTR]
;2. cli 
cli
;3. open A20
in al, 0x92
or al, 0x02
out 0x92, al
;protect open
mov eax, cr0
or eax, 0x01
mov cr0, eax
;jump 32bits code , use descriptor 1 
;jmp dword code32_selector : 0
;push stack32_selector
;push stack32_limit
push code32_selector
push 0x0000
retf


RET_ENTRY:
    mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, stack_top
    
    in al, 0x92
    and al, 0xfd
    out 0x92, al  
    sti

    mov bp, str_test
    mov cx, 0x0c
    mov dx, 0x1400
    mov bx, 0x00f0
    mov ax, 0x1301
    int 0x10

    jmp $
;============ system function =============;
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


;name get_memory_info_e801
;return in eax:0==>success, 1==>false
get_memory_info_e801:
    push eax
	push ebx
	mov eax, 0xe801
	int 0x15
    
	shl eax, 10   ;1kb
	shl ebx, 10+6 ;64kb
	add eax, 0x100000 ;15mb-16mb
	add eax, ebx

    mov [MEM_SIZE], eax
	xor eax, eax
    jnc success_run 
	mov eax, 1
    
success_run:
    pop ebx 
	pop eax
    ret

;name init memory infomation
;return in eax:0==>success, 1==>false
init_memory_info:
    push eax
	push ebx
	push ecx
	push edx
	push edi

    call get_memory_info_e801

	mov ebx, 0
    mov edi, ARDS_ARR 
next_ards:
    mov eax, 0xe820
	mov ecx, 20
    mov edx, 0x0534D4150

	int 0x15
	add edi, 0x14
	inc dword [MEM_ARDS_COUNT]
    cmp ebx, 0
	jnz next_ards

; found max mem size
    mov edi, ARDS_ARR 
	mov ax, sys_info_selector
    mov es, ax
    mov ecx, dword [MEM_ARDS_COUNT]
    
found_loop_:
	mov ebx, [es:edi+16]
	cmp ebx, 0x01
    jne next_value

    mov eax, dword [es:edi]
	add eax, dword [es:edi+8]
    cmp eax, dword [MEM_SIZE]
	jb next_value
    mov dword [MEM_SIZE], eax

next_value:
    add edi, 20 
	loop found_loop_

return_value: mov eax, 0
    jnc get_mem_run_over
    mov eax, 1
get_mem_run_over:
    
	pop edx
	pop edx
	pop ecx
	pop ebx
	pop eax
    ret



[section .s16]
[bits 16]
code16_start:
    ; 1. flush segment register(cache)
	; 2. exit protect mode
    mov ax, update_selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

    mov eax, cr0
	and ax, 0xfffe
	mov cr0, eax

ret_to_real:
    jmp  0:RET_ENTRY

code16_len  equ    $-code16_start

; ************************* TSS *******************************
[section .tss]
[bits 32]
tss_start:
    dd 0
	; level 0 stack information
    dd stack32_limit
    dd stack32_selector
	; level 1
    dd 0
	dd 0
	; level 2
	dd 0
	dd 0

    ;register
	times 4*18 dd 0
	; ... ...
	dw 0
	dw $ - tss_start - 2
	;end tss struct flag
	db 0xff

tss_len    equ         $ - tss_start


[section .rdata_code]
[bits 32]
rdata_start:
;function define.
; argument in ecx, return sqr in eax
sqr:
    mov eax, ecx
	mul eax
    retf
sqr_off  equ    sqr - $$
sqr_len  equ     $  - sqr

; argument in ecx, return add 1 + 2 + ... + n
add:
    push ecx
    xor eax, eax

add_loop:
    add eax, ecx
    loop add_loop

    pop ecx
    retf
add_off  equ    add - $$
add_len  equ     $  - add

rdata_len  equ    $ - rdata_start
rdata_off  equ    rdata_start - $$
;===========================  32_function =============================
[section .my_function]
[bits 32]
function_seg_start:

; name: print_info
; string end of '\0'
; src in --> (DS:ESI)
; attribute-> Bh
; row in  DH 
; cols in DL
print_info:
	push edi
	push eax
    push bx
	push dx

    mov eax, 80
	mul dh
	and dx, 0x00ff
	add ax, dx
	shl eax, 1 
	mov edi, eax

    mov ax, video_selector
	mov gs, ax
	xor ax, ax

	mov ah, bh
    
print_ainfo:
    mov al, byte [ds:esi]
	cmp al, 0
	je  end_print

    mov [gs:edi], ax ; print to show cache
	inc esi
	add edi, 2
    jmp print_ainfo

end_print:
    pop dx
	pop bx
    pop eax 
	pop edi
    retf
print_info_off  equ       print_info - $$


; name: acce_kernel_data
; ds:esicopy to es:edi
; patch: check ds rpl.
acce_kernel_data:
    ;push edi
    ;push eax
	;push ecx

    mov cx, word [esp + 4]
	mov ax, es
    or  ax, cx

	call check_rpl

	cld
    
	mov ax, data_selector
	mov ds, ax

	mov ecx, test_len 
    mov esi, str_offset 
	rep movsb


    ;pop ecx
    ;pop eax
    ;pop edi
    retf
acce_kernel_off  equ     acce_kernel_data - $$

; name: check_rpl
; arguments in  ax
check_rpl:
    push ax

    and ax, 0x03
	cmp ax, SA_RPL0
    je legal_acce

illegal_acce dd  0xdeadc0de
	; exception code in here.
	mov ax, 0x00
	mov fs, ax
	mov ax, 0x0dead
	mov word [fs:0], ax

legal_acce:
    pop ax
	ret
;=============================== ;

;name: set_up_page
; page table director selector in eax
; page table selector in          ebx
; page table     base in          ecx
init_page_table:
    push edi
    push es
	push ecx
	push ebx
	push eax

	mov es, ax
	xor edi, edi

	mov eax, [esp + 8]
    or  eax, PG_P | PG_USU | PG_RWW
    mov ecx, 0x400
    cld

page_dir_init:
    stosd
	add eax, 0x1000
    loop page_dir_init

    mov ax, word [esp + 4]
	mov es, ax   ;!!! err.228:forget here
	xor edi, edi
	mov eax, PG_P | PG_USU | PG_RWW
	mov ecx, 0x100000 ;1024 * 1024
	cld

page_tbl_init:
    stosd
    add eax, 0x1000
	loop page_tbl_init
    
end_:
    pop eax
	pop ebx
	pop ecx
	pop es
	pop edi
    retf  
   
page_tbl_init_off    equ        init_page_table - $$

;name : switch_page_table
;argument: page_dir_base in eax

switch_page_table:
    push eax
	; close page mode
	mov eax, cr0
	and  eax, (0x7fffffff)
	mov cr0, eax

    mov eax, [esp]
	mov cr3, eax
    mov eax, cr0
	or  eax, (0x01 << 31)
	mov cr0, eax
    pop eax
	retf
page_tbl_switch_off    equ       switch_page_table - $$

function_len   equ        $ - function_seg_start
;******************************************** 32 ********************************************;
[section .s32]
[bits 32]
code32_seg_start:
    
	mov ax, stack32_selector
	mov ss, ax
	mov esp, stack32_limit


	mov ax, data_selector
	mov ds, ax
    mov esi, OS_offset 
    mov bh, 0x0c
	mov dx, 0x1100
	
	call func_print_selector : 0
     
	mov ax, data_selector
	mov ds, ax
    mov esi, str_offset 
    mov bh, 0x0c
	mov dx, 0x1200
	
	call func_print_selector : 0    

    mov ax, tss_selector
	ltr ax

    mov ax, taska_ldt_selector
	lldt ax
; some data initilaze operator
    mov ax, flat_mode_selector
	mov es, ax
	mov ax, rdata_selector
	mov ds, ax

	mov cx, sqr_len
	mov esi, sqr_off
	mov edi, 0x1A01000  ;由于存在多个页表，因此拷贝的地址需要避开到0xA02000之后
	call mem_copy32
	
	mov cx, add_len
    mov esi, add_off
	mov edi, 0x1B01000  ;因此拷贝数据到了一个遥远的地点。
	call mem_copy32

debug_code:	nop
            nop
			nop
; page table init
    mov eax, page_dir_selector0
	mov ebx, page_tbl_selector0
	mov ecx, page_tbl_base0
	call init_page_tbl_selector : 0 

    mov eax, page_dir_selector1
	mov ebx, page_tbl_selector1
	mov ecx, page_tbl_base1
	call init_page_tbl_selector : 0 

 
; change mapping
    mov ax, flat_mode_selector
	mov es, ax
	mov eax, 0x401000
	mov ebx, 0x1A01000
	mov ecx, page_dir_base0
    call map_address

; change mapping
    mov ax, flat_mode_selector
	mov es, ax
	mov eax, 0x401000
	mov ebx, 0x1B01000
	mov ecx, page_dir_base1
    call map_address

; chooose page table zero    
	mov eax, page_dir_base0
	call switch_page_tbl_selector : 0
; run func on page table zero: 0x401000
    mov ecx, 9
	call flat_code_selector : 0x401000

; choose page table one
	mov eax, page_dir_base1
	call switch_page_tbl_selector : 0
; run func on page table one: 0x401000
    mov ecx, 10
    call flat_code_selector:0x401000

	;jmp word  taska_code32_selector : 0 
	push taska_stack32_selector
	push taska_stack32_top
	push taska_code32_selector
	push 0x0000
	retf

; ===================== 32 kernel code function.temp.
;name: mem_cpy32
;ds:esi: --> sourse
;es:edi: --> destination
;ecx:    --> lenth
mem_copy32:
    rep movsb
    ret
;Is it need function?

;name:map_address
;es --> falt mode selector
;eax ==> virtual address
;ebx ==> target address
;ecx ==> page directory base
; virtual addr:0x401000 , k=1, j=1. map_addr in  [[dir_base + k*4]+j*4] 
map_address:
    shr eax, 12
    push eax

	;1. get k
	shr eax, 10 ;k
	shl eax, 2; k*4

	;2. get page dir table start
	and ecx, 0xfffff000 ; clean addr attribute
    add eax, ecx
	mov edi, eax
    mov edi, dword [es:edi]
    and edi, 0xfffff000

	;3. get j and page table addr
	pop eax
	and eax, 0x3ff;j
	shl eax, 2;j*4
    
	add edi, eax; pointer
    ;4. change target address
	and ebx, 0xfffff000
	or ebx, PG_P | PG_USU | PG_RWW
    mov [es:edi], ebx
    
    ret

code32_len  equ    $-code32_seg_start


; data segments
[section .dat]
[bits 32]
data32_start:
str_test db "k_hello world!", 0
test_len   equ $ - data32_start

str_OS_start:
str_OS   db "OS Loader!", 0
OS_len: equ  $ - str_OS_start

str_offset equ data32_start - $$
OS_offset  equ str_OS - $$

data32_len equ $ - data32_start

; globl stack segment .gs
[section .gs]
[bits 32]
stack32_start:
    times 4096 db 0 
stack32_len   equ $-stack32_start
stack32_limit equ stack32_len-1


; ======================================TASK_A========================================== ;

[section .taska_data32]
[bits 32]
taska_data_start:
    taska_info    db       "taska running!", 0
taska_info_offset    equ         taska_data_start - $$
taska_data32_len     equ         $ - taska_data_start


[section .taska_stack32]
[bits 32]
taska_stack_start:
    times  1024    db    0

taska_stack32_len    equ         $ - taska_stack_start
taska_stack32_top    equ         taska_stack32_len - 1

[section .taska_code32]
[bits 32]
taska_code_start:
	mov ax, taska_data32_selector
    
	;illegal operator owner private
	and ax, 0x0fffc

    mov es, ax
   
    ;;want get kernel data
    ;mov edi, taska_info_offset
	;call func_acce_selector : 0

    mov ax, taska_data32_selector
    mov ds, ax
    
	mov esi, taska_info_offset
	mov bh, 0x0c
	mov dx, 0x1300
	call func_print_selector : 0

    jmp $
	;jmp code16_selector : 0

taska_code32_len    equ         $-taska_code_start


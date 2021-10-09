	.file	"sys_call.c"
	.text
	.globl	sys_exit
	.type	sys_exit, @function
sys_exit:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
#APP
# 7 "sys_call.c" 1
	xorl %eax, %eax
	int  $0x80   
	
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	sys_exit, .-sys_exit
	.globl	create_mutex
	.type	create_mutex, @function
create_mutex:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	leaq	-12(%rbp), %rsi
#APP
# 17 "sys_call.c" 1
	movl $1, %eax
	xorl %ebx, %ebx
	movl %rsi, %ecx
	int      $0x80
	
# 0 "" 2
#NO_APP
	movl	-12(%rbp), %eax
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	create_mutex, .-create_mutex
	.globl	enter_critical
	.type	enter_critical, @function
enter_critical:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ecx
#APP
# 32 "sys_call.c" 1
	movl $1, %eax
	movl $1, %ebx
	int      $0x80
	
# 0 "" 2
#NO_APP
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	enter_critical, .-enter_critical
	.globl	exit_critical
	.type	exit_critical, @function
exit_critical:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ecx
#APP
# 45 "sys_call.c" 1
	movl $1, %eax
	movl $2, %ebx
	int      $0x80
	
# 0 "" 2
#NO_APP
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	exit_critical, .-exit_critical
	.globl	destory_mutex
	.type	destory_mutex, @function
destory_mutex:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	movl	%edi, -12(%rbp)
	movl	-12(%rbp), %ecx
#APP
# 57 "sys_call.c" 1
	movl $1, %eax
	movl $3, %ebx
	int      $0x80
	
# 0 "" 2
#NO_APP
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	destory_mutex, .-destory_mutex
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits

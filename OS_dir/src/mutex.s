	.file	"mutex.c"
	.text
	.globl	mutex_call_handler
	.type	mutex_call_handler, @function
mutex_call_handler:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -12(%rbp)
#APP
# 7 "mutex.c" 1
	pushl %ebx
	pushl %ecx
	pushl %edx
	movl  %ebx, -4(%rbp)
	movl  %ecx, -8(%rbp)
	movl  %edx, -12(%rbp)
	popl  %edx
	popl  %ecx
	popl  %ebx
	
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	mutex_call_handler, .-mutex_call_handler
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits

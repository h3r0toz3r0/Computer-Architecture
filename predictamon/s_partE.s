.file	"partE.c"
	.option nopic
	.text
	.align	2
	.globl	partE
	.type	partE, @function
partE:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
.L2:
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	li	a6,99
	add	a4,a5,a4
	addi	a5,a5,1				; Register Mappings
	sw	a4,-36(s0)				; a4 => -36(s0) => %ebx
	sw	a5,-40(s0)				; a5 => -40(s0) => %ecx
	ble	a5,a6,.L2
	j	.L3
.L3:
	lw	a5,-36(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	partE, .-partE
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	li	a5,0
	sw	a5,-20(s0)
	li	a5,0
	sw	a5,-24(s0)
	lw	a1,-24(s0)
	lw	a0,-20(s0)
	call	partE
	sw	a0,-28(s0)
	lw	a5,-28(s0)
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 7.2.0"

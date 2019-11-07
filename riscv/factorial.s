	.file	"factorial.c"			; run in BRISC-V Simulator
	.option nopic
	.text
	.align	2
	.globl	multiply_by_add
	.type	multiply_by_add, @function
multiply_by_add:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	sw	a1,-40(s0)
	lw	a4,-36(s0)
	lw	a5,-40(s0)
	li	a6,1
	lw	a7,-36(s0)
.L2:
	bne	a5,a6,.L3
	lw	a5,-36(s0)
	sw	a5,-20(s0)
	j	.L5
.L3: 
	sub	a5,a5,a6
	add	a4,a7,a4
	sw	a4,-36(s0)
	j	.L2
.L5:
	lw	a5,-20(s0)
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	multiply_by_add, .-multiply_by_add
	.align	2
	.globl	factorial
	.type	factorial, @function
factorial:
	addi	sp,sp,-48
	sw	ra,24(sp)
	sw	s0,20(sp)
	addi	s0,sp,48
	sw	a0,-20(s0)
	lw	a4,-20(s0)
	li	a5,1
	bgt	a4,a5,.L6
	lw	a5,-20(s0)
	j	.L7
.L6:
	lw	a5,-20(s0)
	addi	a5,a5,-1
	mv	a0,a5
	call	factorial
	mv	a1,a0
	lw	a0,-20(s0)
	call	multiply_by_add
	mv	a5,a0
.L7:
	mv	a0,a5
	lw	ra,24(sp)
	lw	s0,20(sp)
	addi	sp,sp,48
	jr	ra
	.size	factorial, .-factorial
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	ra,28(sp)
	sw	s0,24(sp)
	addi	s0,sp,32
	li	a5,6
	sw	a5,-20(s0)
	lw	a0,-20(s0)
	call	factorial
	sw	a0,-24(s0)
	lw	a5,-24(s0)
	mv	a0,a5
	lw	ra,28(sp)
	lw	s0,24(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 7.2.0"

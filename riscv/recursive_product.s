.file   "product.c"			; run in BRISC-V Simulator
    .option nopic
    .text
    .align  2
    .globl  product
    .type   product, @function
product:
    addi    sp,sp,-48
    sw  ra,44(sp)
    sw  s0,40(sp)
    addi    s0,sp,48
    mv  a5,a0
    mv  a4,a1
    sw  a5,-36(s0)
    mv  a5,a4
    sw  a5,-40(s0)
    bne a5,zero,.L2
    li  a5,0
    j   .L3
.L2:
    lw  a5,-40(s0)
    addi    a5,a5,-1
    sw  a5,-40(s0)
    lw  a4,-40(s0)
    lw  a5,-36(s0)
    mv  a1,a4
    mv  a0,a5
    call product
    mv  a5,a0
    mv  a4,a5
    lw  a5,-36(s0)
    add a5,a5,a4
.L3:
    mv  a0,a5
    lw  ra,44(sp)
    lw  s0,40(sp)
    addi    sp,sp,48
    jr  ra
    .size   product, .-product
    .align  2
    .globl  main
    .type   main, @function
main:
    addi    sp,sp,-32
    sw  ra,28(sp)
    sw  s0,24(sp)
    addi    s0,sp,32
    li  a5,4
    sw  a5,-20(s0)
    li  a5,9
    sw  a5,-24(s0)
    lw  a1,-24(s0)
    lw  a0,-20(s0)
    call    product
    sw  a0,-28(s0)
    lw  a5,-28(s0)
    mv  a0,a5
    lw  ra,28(sp)
    lw  s0,24(sp)
    addi    sp,sp,32
    jr  ra
    .size   main, .-main
    .ident  "GCC: (GNU) 7.2.0"
    addi    zero,zero,0
    addi    zero,zero,0
    addi    zero,zero,0
    addi    zero,zero,0
    auipc   ra,0x0     
    jalr    ra,0(ra)   
    addi    zero,zero,0
    addi    zero,zero,0
    addi    zero,zero,0
    addi    zero,zero,0

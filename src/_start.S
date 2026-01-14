.global _start
.type _start, @function
.extern load
_start:
    mov	x0, sp // x0->argc
    ldr	x1, [x0],#8 // x0->argv
    add	x0, x0, w1, sxtw #3
    add	x0, x0, #0x8 // x0->envp
    envp:
    ldr	x1, [x0],#8
    cbnz	x1, envp
    bl load
    br x0

.global _svc
.type _svc, @function
_svc:
    mov x8, x0
    mov x0, x1
    mov x1, x2
    mov x2, x3
    mov x3, x4
    mov x4, x5
    mov x5, x6
    svc #0
    ret
    

global _start
_start:
    mov rax, 1
    mov rdi, 1
    lea rsi, [msg]
    mov rdx, msgLen
    syscall

    mov rax, 60
    mov rdi, 0
    syscall

section .data
    msg: db "Working", 10
    msgLen: equ $ - msg
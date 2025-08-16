global main
extern GetStdHandle
extern WriteConsoleA
extern ExitProcess

section .data
    msg db "Working", 10
    msgLen equ $ - msg
    STD_OUTPUT_HANDLE equ -11

section .text
main:
    sub rsp, 28h            ; shadow space + alignment
    mov ecx, STD_OUTPUT_HANDLE
    call GetStdHandle
    mov r8, rax             ; save handle

    ; WriteConsoleA(hStdOut, msg, msgLen, &written, NULL)
    lea rcx, [r8]           ; handle
    lea rdx, [rel msg]      ; RIP-relative address!
    mov r8d, msgLen         ; length
    sub rsp, 20h            ; shadow space for call
    lea r9, [rsp]            ; LPVOID lpNumberOfCharsWritten
    xor rax, rax            ; NULL for lpReserved
    call WriteConsoleA
    add rsp, 28h

    ; ExitProcess(0)
    xor ecx, ecx
    call ExitProcess

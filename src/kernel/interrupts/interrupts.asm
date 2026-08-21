bits 64

section .text

extern exception_handler
extern keyboard_handler

global exception_0
global exception_6
global exception_8
global exception_13
global exception_14

global keyboard_interrupt


; --------------------------------
; Exceptions WITHOUT CPU error code
; --------------------------------

exception_0:
    push qword 0
    push qword 0
    jmp exception_common

exception_6:
    push qword 0
    push qword 6
    jmp exception_common


; --------------------------------
; Exceptions WITH CPU error code
; --------------------------------

exception_8:
    push qword 8
    jmp exception_common

exception_13:
    push qword 13
    jmp exception_common

exception_14:
    push qword 14
    jmp exception_common


; --------------------------------
; Common exception bridge
; --------------------------------

exception_common:
    cli

    mov rdi, [rsp]
    mov rsi, [rsp + 8]

    and rsp, -16

    call exception_handler

.halt:
    cli
    hlt
    jmp .halt


; --------------------------------
; Keyboard IRQ 1
; --------------------------------

keyboard_interrupt:
    ; Hardware interrupts can happen at any point,
    ; so preserve the interrupted code's registers.

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Preserve the exact interrupt-stack location
    ; while satisfying the C++ ABI alignment.
    mov r12, rsp
    and rsp, -16

    call keyboard_handler

    mov rsp, r12

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq
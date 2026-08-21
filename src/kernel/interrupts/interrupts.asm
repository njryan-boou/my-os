bits 64

section .text

extern exception_handler

global exception_0
global exception_6
global exception_8
global exception_13
global exception_14


; --------------------------------
; Exceptions WITHOUT CPU error code
; --------------------------------

exception_0:
    push qword 0        ; synthetic error code
    push qword 0        ; vector
    jmp exception_common

exception_6:
    push qword 0        ; synthetic error code
    push qword 6        ; vector
    jmp exception_common


; --------------------------------
; Exceptions WITH CPU error code
; --------------------------------
;
; For these, the CPU has already pushed an error code.
; We only push the vector number.
;

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
; Common bridge to C++
; --------------------------------

exception_common:
    cli

    ; System V x86-64 calling convention:
    ;
    ; RDI = first argument
    ; RSI = second argument

    mov rdi, [rsp]        ; vector
    mov rsi, [rsp + 8]    ; error code

    ; We're never returning, so we can safely realign
    ; the stack before entering C++.
    and rsp, -16

    call exception_handler

.halt:
    cli
    hlt
    jmp .halt
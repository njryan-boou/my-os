bits 64

section .text

global invalid_opcode_stub
extern invalid_opcode_handler

invalid_opcode_stub:
    ; We never return from the C++ panic handler,
    ; so we can align the stack directly.
    and rsp, -16

    call invalid_opcode_handler

.halt:
    cli
    hlt
    jmp .halt
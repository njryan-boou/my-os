bits 64

section .text.entry

global kernel_entry

extern kernel_main
extern __bss_start
extern __bss_end


kernel_entry:
    ; -------------------------
    ; Zero the .bss section
    ; -------------------------

    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi

    xor eax, eax

    ; Make sure string operations move forward.
    cld

    ; Write AL (0) RCX times starting at RDI.
    rep stosb


    ; -------------------------
    ; Enter C++
    ; -------------------------

    ; Establish ABI-friendly stack alignment.
    and rsp, -16

    call kernel_main


    ; kernel_main should never return.
.halt:
    cli
    hlt
    jmp .halt
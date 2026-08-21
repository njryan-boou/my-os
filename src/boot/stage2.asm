bits 16
org 0x1000

stage2:
    cli

    ; Establish known segment state
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Give stage 2 its own stack
    mov sp, 0x9000

    ; Load our GDT
    lgdt [gdt_descriptor]

    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Enter our 32-bit code segment
    jmp 0x08:protected_mode


; -------------------------
; Global Descriptor Table
; -------------------------

gdt_start:

gdt_null:
    dq 0x0000000000000000

gdt_code:
    dq 0x00cf9a000000ffff

gdt_data:
    dq 0x00cf92000000ffff

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; -------------------------
; 32-bit protected mode
; -------------------------

bits 32

protected_mode:
    ; Load data-segment selector
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Establish a 32-bit stack
    mov esp, 0x9000

    ; Print directly through VGA memory
    mov esi, message
    mov edi, 0xb8000

    call print32


.halt:
    cli
    hlt
    jmp .halt


; -------------------------
; VGA print routine
; -------------------------

print32:
    lodsb
    test al, al
    jz .return

    mov ah, 0x0f
    mov word [edi], ax

    add edi, 2
    jmp print32

.return:
    ret


message:
    db "Stage 2: Protected Mode", 0


; Fill one complete disk sector
times 512 - ($ - $$) db 0
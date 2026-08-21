bits 16
org 0x7c00


; -------------------------
; 16-bit boot entry
; -------------------------

start:
    cli

    ; Initialize data segment and stack
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00

    sti

    ; Load our Global Descriptor Table
    lgdt [gdt_descriptor]

    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump loads our code segment and enters 32-bit code
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


; Structure consumed by LGDT
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; -------------------------
; 32-bit protected mode
; -------------------------

bits 32

protected_mode:
    ; 0x10 selects our GDT data descriptor
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; ESI points to our string
    mov esi, message

    ; VGA text-mode memory
    mov edi, 0xb8000

    call print32


.halt:
    cli
    hlt
    jmp .halt


; -------------------------
; 32-bit VGA print routine
; -------------------------

print32:
    ; Read next character into AL
    lodsb

    ; Null terminator means end of string
    test al, al
    jz .return

    ; VGA attribute: white text on black background
    mov ah, 0x0f

    ; Write character + attribute to VGA memory
    mov word [edi], ax

    ; Each VGA text cell is 2 bytes
    add edi, 2

    jmp print32


.return:
    ret


; -------------------------
; Data
; -------------------------

message:
    db "Hello World!", 0


; -------------------------
; Boot sector
; -------------------------

; Pad to 510 bytes
times 510 - ($ - $$) db 0

; BIOS boot signature
dw 0xaa55
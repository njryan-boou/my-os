bits 16
org 0x1000


; -------------------------
; Stage 2 entry
; -------------------------

stage2:
    cli

    ; Establish known segment state
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Temporary 16-bit stack
    mov sp, 0x9000

    ; Load Global Descriptor Table
    lgdt [gdt_descriptor]

    ; Enable protected mode: CR0.PE = 1
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Enter 32-bit protected mode using GDT selector 0x08
    jmp 0x08:protected_mode


; -------------------------
; Global Descriptor Table
; -------------------------

gdt_start:

; 0x00 - null descriptor
gdt_null:
    dq 0x0000000000000000

; 0x08 - 32-bit code segment
gdt_code32:
    dq 0x00cf9a000000ffff

; 0x10 - data segment
gdt_data:
    dq 0x00cf92000000ffff

; 0x18 - 64-bit code segment
gdt_code64:
    dq 0x00af9a000000ffff

gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; -------------------------
; 32-bit protected mode
; -------------------------

bits 32

protected_mode:
    ; Load data segment selector
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 32-bit stack
    mov esp, 0x9000


    ; -------------------------
    ; Build page tables
    ; -------------------------

    ; PML4[0] -> PDPT at 0x11000
    ; Present | Writable
    mov dword [0x10000], 0x11003
    mov dword [0x10004], 0

    ; PDPT[0] -> Page Directory at 0x12000
    ; Present | Writable
    mov dword [0x11000], 0x12003
    mov dword [0x11004], 0

    ; PD[0] -> identity-map first 2 MiB
    ; Present | Writable | Huge Page
    mov dword [0x12000], 0x00000083
    mov dword [0x12004], 0


    ; -------------------------
    ; Configure paging
    ; -------------------------

    ; CR3 points to the PML4
    mov eax, 0x10000
    mov cr3, eax

    ; Enable PAE: CR4.PAE = 1
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax


    ; -------------------------
    ; Enable long mode
    ; -------------------------

    ; Select IA32_EFER MSR
    mov ecx, 0xc0000080

    ; Read EFER into EDX:EAX
    rdmsr

    ; Set EFER.LME (Long Mode Enable), bit 8
    or eax, 1 << 8

    ; Write EFER back
    wrmsr


    ; -------------------------
    ; Enable paging
    ; -------------------------

    mov eax, cr0

    ; Set CR0.PG, bit 31
    or eax, 1 << 31

    mov cr0, eax


    ; -------------------------
    ; Enter 64-bit code
    ; -------------------------

    ; 0x18 selects our 64-bit code descriptor
    jmp 0x18:long_mode


; -------------------------
; 64-bit long mode
; -------------------------

bits 64

long_mode:
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, 0x9000

    ; Enter the C++ kernel
    call 0x2000

.halt:
    cli
    hlt
    jmp .halt


; -------------------------
; Stage 2 padding
; -------------------------

; Stage 2 occupies exactly 8 sectors = 4096 bytes
times 4096 - ($ - $$) db 0
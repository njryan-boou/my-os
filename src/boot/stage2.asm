bits 16
org 0x1000


; -------------------------
; Memory map locations
; -------------------------

MEMORY_MAP_COUNT equ 0x4ff0
MEMORY_MAP       equ 0x5000

E820_ENTRY_SIZE  equ 24
E820_MAX_ENTRIES equ 64


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

    ; Ask BIOS for the complete physical memory map.
    call detect_memory

    ; Load Global Descriptor Table.
    lgdt [gdt_descriptor]

    ; Enable protected mode: CR0.PE = 1.
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Enter 32-bit protected mode.
    jmp 0x08:protected_mode


; -------------------------
; BIOS E820 memory detection
; -------------------------

detect_memory:
    ; EBX = 0 means this is the first E820 request.
    xor ebx, ebx

    ; BP counts the entries we keep.
    xor bp, bp

    ; BIOS writes the first entry to ES:DI.
    mov di, MEMORY_MAP


.next_entry:
    ; Prevent the BIOS from overflowing our buffer.
    cmp bp, E820_MAX_ENTRIES
    jae .done

    ; Select BIOS E820.
    mov eax, 0xe820

    ; Required "SMAP" signature.
    mov edx, 0x534d4150

    ; Request a 24-byte entry.
    mov ecx, E820_ENTRY_SIZE

    ; Initialize ACPI extended attributes.
    mov dword [es:di + 20], 1

    ; Request the next memory-map entry.
    int 0x15

    ; Carry indicates failure/end.
    jc .done

    ; BIOS must return the SMAP signature.
    cmp eax, 0x534d4150
    jne .done

    ; -------------------------
    ; Ignore zero-length regions
    ; -------------------------

    ; Length is the 64-bit value at offsets 8-15.
    mov eax, [es:di + 8]
    or eax, [es:di + 12]

    jz .skip_entry


    ; -------------------------
    ; Keep this entry
    ; -------------------------

    inc bp

    ; Move ES:DI to the next 24-byte slot.
    add di, E820_ENTRY_SIZE


.skip_entry:
    ; BIOS returns a continuation value in EBX.
    ;
    ; EBX = 0 means there are no more entries.
    test ebx, ebx
    jnz .next_entry


.done:
    ; Make the final count available to the kernel.
    mov [MEMORY_MAP_COUNT], bp

    ret


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
    ; Load data segment.
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Establish 32-bit stack.
    mov esp, 0x9000


    ; -------------------------
    ; Build page tables
    ; -------------------------

    ; PML4[0] -> PDPT at 0x11000
    ;
    ; Present | Writable
    mov dword [0x10000], 0x11003
    mov dword [0x10004], 0

    ; PDPT[0] -> Page Directory at 0x12000
    ;
    ; Present | Writable
    mov dword [0x11000], 0x12003
    mov dword [0x11004], 0

    ; PD[0] -> identity-map the first 2 MiB.
    ;
    ; Present | Writable | Huge Page
    mov dword [0x12000], 0x00000083
    mov dword [0x12004], 0


    ; -------------------------
    ; Configure paging
    ; -------------------------

    ; CR3 points to our PML4.
    mov eax, 0x10000
    mov cr3, eax

    ; Enable PAE: CR4.PAE = 1.
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax


    ; -------------------------
    ; Enable long mode
    ; -------------------------

    ; Select IA32_EFER.
    mov ecx, 0xc0000080

    ; Read EFER into EDX:EAX.
    rdmsr

    ; Set EFER.LME.
    or eax, 1 << 8

    ; Write EFER back.
    wrmsr


    ; -------------------------
    ; Enable paging
    ; -------------------------

    mov eax, cr0

    ; Set CR0.PG.
    or eax, 1 << 31

    mov cr0, eax


    ; -------------------------
    ; Enter 64-bit mode
    ; -------------------------

    ; 0x18 selects our 64-bit code descriptor.
    jmp 0x18:long_mode


; -------------------------
; 64-bit long mode
; -------------------------

bits 64

long_mode:
    ; Load data segment.
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Establish 64-bit stack.
    mov rsp, 0x9000

    ; C++ kernel is loaded at physical address 0x2000.
    call 0x2000


; -------------------------
; Halt
; -------------------------

.halt:
    cli
    hlt
    jmp .halt


; -------------------------
; Stage 2 padding
; -------------------------

; Stage 2 occupies exactly 8 sectors = 4096 bytes.
times 4096 - ($ - $$) db 0
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

    ; Establish known segment state.
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Temporary real-mode stack.
    mov sp, 0x9000

    sti

    ; Ask BIOS for the physical memory map.
    call detect_memory

    ; Disable interrupts before changing CPU modes.
    cli

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
    ; EBX = 0 means this is the first request.
    xor ebx, ebx

    ; BP counts the entries we keep.
    xor bp, bp

    ; BIOS writes entries to ES:DI.
    mov di, MEMORY_MAP


.next_entry:
    ; Prevent buffer overflow.
    cmp bp, E820_MAX_ENTRIES
    jae .done

    ; BIOS E820 function.
    mov eax, 0xe820

    ; Required "SMAP" signature.
    mov edx, 0x534d4150

    ; Request a 24-byte entry.
    mov ecx, E820_ENTRY_SIZE

    ; Initialize ACPI extended attributes.
    mov dword [es:di + 20], 1

    int 0x15

    ; Carry indicates failure/end.
    jc .done

    ; Verify returned "SMAP" signature.
    cmp eax, 0x534d4150
    jne .done


    ; -------------------------
    ; Ignore zero-length regions
    ; -------------------------

    mov eax, [es:di + 8]
    or eax, [es:di + 12]

    jz .skip_entry


    ; -------------------------
    ; Keep this entry
    ; -------------------------

    inc bp
    add di, E820_ENTRY_SIZE


.skip_entry:
    ; EBX = 0 means there are no more entries.
    test ebx, ebx
    jnz .next_entry


.done:
    ; Expose count to the kernel.
    mov [MEMORY_MAP_COUNT], bp

    ret


; -------------------------
; Global Descriptor Table
; -------------------------

gdt_start:


; 0x00 - null descriptor
gdt_null:
    dq 0x0000000000000000


; -------------------------
; 0x08 - 32-bit code
; -------------------------

gdt_code32:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00


; -------------------------
; 0x10 - data
; -------------------------

gdt_data:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00


; -------------------------
; 0x18 - 64-bit code
; -------------------------

gdt_code64:
    dw 0xffff
    dw 0x0000
    db 0x00
    db 10011010b
    db 10101111b
    db 0x00


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


    ; -------------------------
    ; Identity-map first 1 GiB
    ;
    ; 512 x 2 MiB huge pages
    ; -------------------------

    mov edi, 0x12000
    xor ebx, ebx
    mov ecx, 512


.map_page:
    ; Physical base address.
    mov eax, ebx

    ; Present | Writable | Huge Page
    or eax, 0x83

    mov dword [edi], eax
    mov dword [edi + 4], 0

    ; Next page-directory entry.
    add edi, 8

    ; Next 2 MiB physical region.
    add ebx, 0x200000

    loop .map_page


    ; -------------------------
    ; Load page-table root
    ; -------------------------

    mov eax, 0x10000
    mov cr3, eax


    ; -------------------------
    ; Enable PAE
    ; -------------------------

    mov eax, cr4

    ; CR4.PAE = 1
    or eax, 1 << 5

    mov cr4, eax


    ; -------------------------
    ; Enable long mode
    ; -------------------------

    ; Select IA32_EFER.
    mov ecx, 0xc0000080

    rdmsr

    ; EFER.LME = 1
    or eax, 1 << 8

    wrmsr


    ; -------------------------
    ; Enable paging
    ; -------------------------

    mov eax, cr0

    ; CR0.PG = 1
    or eax, 1 << 31

    mov cr0, eax


    ; -------------------------
    ; Enter 64-bit mode
    ; -------------------------

    ; Selector 0x18 is our
    ; 64-bit code descriptor.
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

    jmp 0x2000

.halt:
    cli
    hlt
    jmp .halt


; -------------------------
; Stage 2 padding
; -------------------------

; Stage 2 occupies exactly
; 8 sectors = 4096 bytes.
times 4096 - ($ - $$) db 0
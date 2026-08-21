bits 16
org 0x7c00

start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov sp, 0x7c00

    sti

    ; BIOS gives us the boot drive in DL.
    mov [boot_drive], dl


    ; -------------------------
    ; Load stage 2
    ;
    ; LBA 1
    ; 8 sectors
    ; destination 0x1000
    ; -------------------------

    mov word [dap.count], 8
    mov word [dap.offset], 0x1000
    mov word [dap.segment], 0x0000
    mov dword [dap.lba_low], 1
    mov dword [dap.lba_high], 0

    call disk_read


    ; -------------------------
    ; Load kernel
    ;
    ; LBA 9
    ; 24 sectors
    ; destination 0x2000
    ; -------------------------

    mov word [dap.count], 24
    mov word [dap.offset], 0x2000
    mov word [dap.segment], 0x0000
    mov dword [dap.lba_low], 9
    mov dword [dap.lba_high], 0

    call disk_read


    ; -------------------------
    ; Enter stage 2
    ; -------------------------

    jmp 0x0000:0x1000


; -------------------------
; Extended BIOS disk read
; -------------------------

disk_read:
    mov ah, 0x42
    mov dl, [boot_drive]

    mov si, dap

    int 0x13
    jc disk_error

    ret


; -------------------------
; Disk error
; -------------------------

disk_error:
    mov si, error_message
    call print

.halt:
    cli
    hlt
    jmp .halt


; -------------------------
; BIOS print routine
; -------------------------

print:
    lodsb

    test al, al
    jz .return

    mov ah, 0x0e
    int 0x10

    jmp print

.return:
    ret


; -------------------------
; Data
; -------------------------

boot_drive:
    db 0

error_message:
    db "Disk read failed!", 0


; -------------------------
; Disk Address Packet
; -------------------------

align 4

dap:
    db 0x10
    db 0

.count:
    dw 0

.offset:
    dw 0

.segment:
    dw 0

.lba_low:
    dd 0

.lba_high:
    dd 0


; -------------------------
; Boot sector
; -------------------------

times 510 - ($ - $$) db 0

dw 0xaa55

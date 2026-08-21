bits 16
org 0x7c00

start:
    cli

    ; Establish known segment state
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Stack grows downward from 0x7C00
    mov sp, 0x7c00

    sti

    ; -------------------------
    ; Load stage 2 from disk
    ; -------------------------

    ; BIOS passes the boot drive in DL.
    ; Stage 2 occupies sectors 2-9.

    mov ah, 0x02        ; BIOS disk read function
    mov al, 8           ; Read 8 sectors

    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start at sector 2
    mov dh, 0           ; Head 0

    mov bx, 0x1000      ; Destination ES:BX = 0000:1000

    int 0x13
    jc disk_error

    ; Stage 2 is now loaded at physical address 0x1000
    jmp 0x0000:0x1000


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


error_message:
    db "Disk read failed!", 0


; Fill exactly one 512-byte boot sector
times 510 - ($ - $$) db 0

; BIOS boot signature
dw 0xaa55
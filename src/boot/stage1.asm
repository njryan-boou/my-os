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

    ; BIOS gave us the boot drive number in DL.
    ; Read stage 2 from disk into memory at 0x1000.

    mov ah, 0x02        ; BIOS disk read
    mov al, 1           ; Read 1 sector

    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2
    mov dh, 0           ; Head 0

    mov bx, 0x1000      ; Destination ES:BX = 0000:1000

    int 0x13

    ; Carry flag means the disk read failed.
    jc disk_error

    ; Stage 2 is now in RAM at 0x1000.
    jmp 0x0000:0x1000


disk_error:
    mov si, error_message
    call print

.halt:
    cli
    hlt
    jmp .halt


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


times 510 - ($ - $$) db 0
dw 0xaa55
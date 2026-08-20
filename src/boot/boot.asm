bits 16
org 0x7c00

start:
    cli

    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7c00

    sti

    mov si, message
    call print

.done:
    cli
    hlt
    jmp .done


print:
    lodsb
    test al, al
    jz .return

    mov ah, 0x0e
    int 0x10
    jmp print

.return:
    ret


message:
    db "Hello World!", 0

times 510 - ($ - $$) db 0
dw 0xaa55
data segment
    s db 100 dup(0)
    t db 100 dup(0)
data ends

code segment
assume cs:code, ds:data

shift:
    sub al, 20h
    mov t[di], al
    add di, 1
    jmp transfer_loop

main:
    mov ax, data
    mov ds, ax
    mov si, 0
    mov di, 0

scan_loop:
    mov ah, 01h
    int 21h
    
    cmp al, 0Dh
    je scan_end

    mov s[si], al
    add si, 1
    jmp scan_loop

scan_end:
    mov s[si], 00h

    mov ah, 02h
    mov dl, 0Dh
    int 21h

    mov ah, 02h
    mov dl, 0Ah
    int 21h

    mov si, 0

transfer_loop:
    mov al, [s+si]
    add si, 1
    cmp al, 00h
    je transfer_end
    
    cmp al, 20h
    je transfer_loop

    cmp al, 61h
    jnb shift

    mov t[di], al
    add di, 1
    jmp transfer_loop

transfer_end:
    mov t[di], 00h
    mov di, 0

print_loop:
    mov dl, [t+di]
    cmp dl, 00h
    je print_end

    mov ah, 02h
    int 21h
    add di, 1
    jmp print_loop

print_end:
    mov ah, 02h
    mov dl, 0Dh
    int 21h

    mov ah, 02h
    mov dl, 0Ah
    int 21h

    mov ah, 4ch
    int 21h
    
code ends
end main                         
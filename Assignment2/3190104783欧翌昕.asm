code segment
assume assume cs:code

main:
    mov ax, data
    mov ds, ax
    mov ax, 0B800h
    mov es, ax
    mov al, 00h
    mov ah, 0Ch
    mov cx, 11
column_loop:
    mov dx, 25
    mov di, 0
row_loop:
    mov byte ptr es:[di], ax


code ends
end main
code segment
assume cs:code

main:
    mov ax, 0B800h
    mov es, ax
    mov bl, 00h
    
    mov ah,15		
	int 10h
	mov ah,0
    int 10h    ;clear screen

column_loop:
    mov dx, 25    ;number of row_loop

row_loop:
    mov al, bl
    mov ah, 0Ch    ;背景为黑色, 前景为高亮度红色
    mov word ptr es:[di], ax
    add di, 2
    mov cx, 2    ;double-digit hex

print_hex:
    push cx
    mov cl, 4
    rol al, cl
    push ax
    and al, 0Fh
    cmp al, 10
    jb is_digit

is_alpha:
   sub al, 10
   add al, 'A'
   jmp finish_4bits

is_digit:
   add al, '0'

finish_4bits:
    mov ah, 0Ah    ;背景为黑色, 前景为高亮度绿色
    mov word ptr es:[di], ax
    add di, 2
   
    pop ax
    pop cx
    sub cx, 1
    jnz print_hex

    add bl, 1
    jz finish

    sub di, 6
    add di, 160    ;change row
    sub dx, 1
    jnz row_loop

    sub di, 160*25
    add di, 14        ;change colume
    jmp column_loop

finish:
    mov ah, 0  ;getchar()
    int 16h

code ends
end main
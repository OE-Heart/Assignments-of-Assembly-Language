data segment
    s db 100 dup(0)
    t db 100 dup(0)
data ends

code segment
assume cs:code, ds:data

shift:
    sub al, 20h  ;Convert lowercase to uppercase
    mov t[di], al  ;Save in array t
    add di, 1 ;(index of d)++
    jmp transfer_loop

main:
    mov ax, data
    mov ds, ax
    mov si, 0
    mov di, 0

scan_loop:
    mov ah, 01h
    int 21h  ;Read in a character
    
    cmp al, 0Dh  ;Determine whether the scanning ends
    je scan_end

    mov s[si], al  ;Save in array s
    add si, 1      ;(index of s)++
    jmp scan_loop

scan_end:
    mov s[si], 00h

    mov ah, 02h
    mov dl, 0Dh
    int 21h  ;Output carriage return

    mov ah, 02h
    mov dl, 0Ah
    int 21h  ;Output line feed

    mov si, 0

transfer_loop:
    mov al, [s+si]
    add si, 1  ;(index of s)++
    cmp al, 00h  ;Detemine whether s ends
    je transfer_end
    
    cmp al, 20h  ;Skip spaces
    je transfer_loop

    cmp al, 61h  ;Determine whether lowercase
    jnb shift

    mov t[di], al  ;Save in array t
    add di, 1  ;(index of d)++
    jmp transfer_loop

transfer_end:
    mov t[di], 00h
    mov di, 0

print_loop:
    mov dl, [t+di]
    cmp dl, 00h
    je print_end

    mov ah, 02h
    int 21h    ;Output a character
    add di, 1  ;(index of d)++
    jmp print_loop

print_end:
    mov ah, 02h
    mov dl, 0Dh
    int 21h  ;Output carriage return

    mov ah, 02h
    mov dl, 0Ah
    int 21h  ;Output line feed

    mov ax, 0  ;getchar()
    int 16h

    mov ah, 4ch
    int 21h
    
code ends
end main                         
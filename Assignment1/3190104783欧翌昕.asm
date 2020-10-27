data segment
    s db 100 dup(0)
    t db 100 dup(0)
data ends
code segment
assume cs:code, ds:data
main:
    mov ax, data
    mov ds, ax
    mov dx, 
    
code ends
end main                         
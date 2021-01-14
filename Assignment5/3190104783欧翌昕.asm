.386
data SEGMENT USE16
    hexTable db "0123456789ABCDEF"
    fileName db 100, ?, 100 dup(0)  ;存储文件名
    s db 100, ?, 100 dup(0)  ;存储待查询字符串
    hex db 100 dup(0)
    hexLen db 4 dup(0)  ;十六进制串中包含的字节数
    buf db 100 dup(0)  ;文件读取Buf
    bufLen db 100, 0, 0, 0  ;buf长度
    fp dw 0  ;文件指针
    n dw 2 dup(0)  ;文件长度
    processed_len dw 2 dup(0)  ;本次已搜索过的字节数
    last_processed_len dw 2 dup(0)  ;上次已搜索过的字节数
    address dd 1 dup(0)  ;查找到所在的地址
    failalert db "Open file failed!$"
    fndalert db "found at $ "
    nfndalert db "Not found!$"
data ENDS

code SEGMENT USE16
ASSUME CS:CODE, DS:DATA

;把十六进制串转化成字节值
transfer:
    push dx
    push cx
    push di
    push si
    mov di, offset hex
    mov si, offset s+2
    mov dl, 0  ;读取的总长

next_num:
    mov cx, 1
    mov bx, 0

next_char:
    mov bl, ds:[si]
    cmp bl, 'A'
    jge is_alpha

is_num:
    sub bl, '0'
    jmp if_this_num_end

is_alpha:
    sub bl, 'A'
    add bl, 0ah

if_this_num_end:
    inc si
    cmp cx, 0
    je this_num_end
    dec cx
    mov bh, bl  ;高4位先存到bh中
    mov bl, 0
    jmp next_char
    
this_num_end:
    shl bh, 4
    add bl, bh
    mov ds:[di], bl
    inc di
    inc dl
    cmp byte ptr ds:[si], 0dh  ;判断是否结束
    je transfer_end
    inc si  ;跳过空格
    jmp next_num

transfer_end:
    mov ds:[hexLen], dl
    pop si
    pop di
    pop cx
    pop dx
    ret

print_address:
    push eax
    push bx
    push cx
    push dx
    push si
    mov eax, ds:[address]
    mov si, 32

print_next:
    push eax
    mov cx, si
    sub cx, 4
    shr eax, cl
    and eax, 0fh
    lea bx, hexTable
    xlat  ;在hexTable表中查找
    mov ah, 02h
    mov dl, al
    int 21h
    pop eax
    sub si, 4
    cmp si, 0
    jne print_next

    pop si
    pop dx
    pop cx
    pop bx
    pop eax
    ret

search:
    mov ebx, dword ptr ds:[n]
    mov eax, dword ptr ds:[processed_len]
    mov dword ptr ds:[last_processed_len], eax
    cmp ebx, eax
    je not_found  ;已读长度已经达到文件长度
    ;设置已读长度
    add eax, dword ptr ds:[bufLen]
    cmp ebx, eax
    jb bufLen_over
    mov cx, word ptr ds:[bufLen]
    sub cx, word ptr ds:[hexLen]
    and ecx, 0ffffh  ;取后16位cx
    add dword ptr ds:[processed_len], ecx  ;更新已读长度
    jmp processed_len_updated

bufLen_over:
    mov eax, dword ptr ds:[processed_len]
    sub ebx, eax
    mov cx, bx  ;cx为剩下的长度
    mov ebx, dword ptr ds:[n]
    mov dword ptr ds:[processed_len], ebx  ;更新已读长度

processed_len_updated:
    ;读取长度为cx的文件
    mov ah, 3Fh
    mov bx, [fp]
    lea dx, buf
    int 21h
    
    mov al, byte ptr ds:[hex] ;起始字符
    mov di, offset buf  ;es:di指向目标字符串
    mov cx, word ptr ds:[bufLen]

search_first_byte:
    cld  ;clear direction flag
    repne scasb  ;汇编语言实现memchr()的功能
    jnz search   ;若未发现起始字符则重新在下一部分中查找
    ;发现起始字符字符，开始字符串比较
    push cx
    push di
    sub di, 1
    mov cx, word ptr ds:[hexLen]
    mov si, offset hex
    repe cmpsb  ;汇编语言实现memcmp()的功能
    je found
    pop di
    pop cx
    jmp search_first_byte

found:
    lea dx, fndalert
    mov ah, 09h
    int 21h
    pop di
    pop cx
    mov eax, dword ptr ds:[last_processed_len] ;已读长度
    add eax, dword ptr ds:[bufLen]
    and ecx, 0ffffh  ;取低16位cx
    sub eax, ecx
    sub eax, 1
    mov dword ptr ds:[address], eax
    call print_address  ;输出地址
    ret

not_found:
    lea dx, nfndalert
    mov ah, 09h
    int 21h
    ret

main:
    mov ax, data
    mov ds, ax
    mov es, ax
    
    ;读入文件名
    lea dx, fileName
    mov ah, 0ah  ;int 21h的0ah号功能，将字符串读取到ds:[dx]
    int 21h
    mov ah, 02h
    mov dl, 0dh
    int 21h
    mov ah, 02h
    mov dl, 0ah
    int 21h
    lea si, fileName
    mov di, ds:[si+1]  ;定位到fileName长度
    and di, 0ffh  ;取后8位
    add di, si
    mov byte ptr ds:[di+2], 0

    ;读入待查询字符串
    lea dx, s
    mov ah, 0ah  ;int 21h的0ah号功能，将字符串读取到ds:[dx]
    int 21h
    mov ah, 02h
    mov dl, 0dh
    int 21h
    mov ah, 02h
    mov dl, 0ah
    int 21h
    
    call transfer  ;把十六进制串转化成字节值存到hex中

    ;汇编语言打开文件步骤:
    mov ah, 3Dh
    mov al, 0
    mov dx, offset fileName+2
    int 21h
    jnb no_error

error:
    lea dx,failalert
    mov ah, 09h
    int 21h
    jmp exit

no_error:
    mov [fp], ax  
    ;汇编语言移动文件指针到EOF:
    mov ah, 42h
    mov al, 2
    mov bx, [fp]
    xor cx, cx
    xor dx, dx
    int 21h; 返回DX:AX为文件长度

    ;汇编语言把文件长度保存到变量n中:
    mov word ptr n[0], ax
    mov word ptr n[2], dx
    mov dword ptr ds:[processed_len], 0
    mov dword ptr ds:[last_processed_len], 0
    
    ;汇编语言移动指针到文件开端
    mov ah, 42h
    mov al, 0
    mov bx, [fp]
    xor cx, cx
    xor dx, dx
    int 21h

    call search
    
exit:
    mov ah, 4ch
    int 21h

code ENDS
end main
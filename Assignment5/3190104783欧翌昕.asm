.386
data SEGMENT USE16
    hexTable db "0123456789ABCDEF"
    filename db 100, ?, 100 dup(0)  ;存储文件名
    s db 100, ?, 100 dup(0)  ;存储待查询字符串
    hex db 100 dup(0)  ;
    hex_len db 4 dup(0)  ;十六进制串中包含的字节数
    dest dd 1 dup(0); 查到所在的地址存在32bit中
    buf db 80 dup(0); 文件读取Buf
    buflen db 80, 0, 0, 0;buf长度，记得和上面的buf保持一样长
    fp dw 0  ;文件指针
    n dw 2 dup(0)  ;文件长度
    processed_len dw 2 dup(0)  ;本次已搜索过的字节数
    last_processed_len dw 2 dup(0)  ;上次已搜索过的字节数
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
    cmp bl, '9'
    jbe is_num

is_alpha:
    sub bl, 'A'
    add bl, 0ah;
    jmp if_this_num_end

is_num:
    sub bl, '0'

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
    cmp byte ptr ds:[si], 0dh;判断是否结束
    je transfer_end
    inc si  ;跳过空格
    jmp next_num

transfer_end:
    mov ds:[hex_len], dl
    pop si
    pop di
    pop cx
    pop dx
    ret

hexopt:
    push eax
    push bx
    push cx
    push dx
    push si
    mov eax, ds:[dest]
    mov bx, offset hexTable
    mov si, 1ch

hexopt_iter_begin:
    push eax
    mov cx, si
    shr eax, cl
    and eax, 0fh
    xlat
    mov ah, 2
    mov dl, al
    int 21h
    pop eax
    cmp si, 0
    je hexopt_iter_end
    sub si, 4
    jmp hexopt_iter_begin

hexopt_iter_end:
    pop si
    pop dx
    pop cx
    pop bx
    pop eax
    ret

main:
    mov ax, data
    mov ds, ax
    mov es, ax
    
    ;读入文件名
    lea dx, filename
    mov ah, 0ah  ;int 21h的0ah号功能，将字符串读取到ds:[dx]
    int 21h
    mov ah, 02h
    mov dl, 0dh
    int 21h  ;Output carriage retuprocessed_len
    mov ah, 02h
    mov dl, 0ah
    int 21h  ;Output line feed
    lea si, filename
    mov di, ds:[si+1]  ;定位到filename长度
    and di, 0ffh  ;取后8位
    add di, si
    mov byte ptr ds:[di+2], 00h

    ;读入待查询字符串
    lea dx, s
    mov ah, 0ah  ;int 21h的0ah号功能，将字符串读取到ds:[dx]
    int 21h
    mov ah, 02h
    mov dl, 0dh
    int 21h  ;Output carriage retuprocessed_len
    mov ah, 02h
    mov dl, 0ah
    int 21h  ;Output line feed
    
    call transfer  ;把十六进制串转化成字节值存到hex中

    ;汇编语言打开文件步骤:
    mov ah, 3Dh
    mov al, 0
    mov dx, offset filename + 2;int21/0ah读入特性
    int 21h
    jc error
    mov [fp], ax; 数据段中要事先定义fp dw 0

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
    
    ;汇编语言移动指针到文件开端
    mov ah, 42h
    mov al, 0
    mov bx, [fp]
    xor cx, cx
    xor dx, dx
    int 21h

    mov dword ptr ds:[processed_len], 0
    mov dword ptr ds:[last_processed_len], 0

next_search_buf:
    ; 汇编语言读文件步骤:
    mov ebx, dword ptr ds:[n]
    mov eax, dword ptr ds:[processed_len]
    mov dword ptr ds:[last_processed_len], eax;存一下上个周期的，方便最后定位找到的地方
    cmp ebx, eax
    je not_found;读完文件还没找到
    ;接下来在设置read_len
    add eax, dword ptr ds:[buflen]
    cmp eax, ebx
    jg buf_overflow
    ;否则正常取cx
    mov cx, word ptr ds:[buflen]
    sub cx, word ptr ds:[hex_len]
    and ecx, 0ffffh;取后16bit 即cx
    add dword ptr ds:[processed_len], ecx;更新已读 减少len是为了保证连续的len个目标不会因为被截断而搜不到
    jmp set_buf_len_done
buf_overflow:
    mov eax, dword ptr ds:[processed_len]
    sub ebx, eax
    mov cx, bx;这时cx为剩下的长度
    mov ebx, dword ptr ds:[n]
    mov dword ptr ds:[processed_len], ebx;更新读完了
set_buf_len_done:
    ;下面的代码会读cx长度的文件到buf中
    mov ah, 3Fh
    mov bx, [fp]
    mov dx, offset buf
    ; add dx, word ptr ds:[buflen]
    int 21h
    
    ;开始搜索前的准备工作
    mov al, byte ptr ds:[hex] ;搜索的16进制头
    ; mov es, ds; main最开始已经做过了
    mov di, offset buf       ;es:di 指向目标字符串
    mov cx,  word ptr ds:[buflen]      ;注意到循环时cx自己会减的 di也自己会加的
in_buf_search:
    cld                         ;方向为正向
    repne scasb                 ;不相等则重复
    jnz next_search_buf                ;若未发现字符则进入下一个循环
    ;若发现了字符 开始比较
    push cx
    push di
    dec di; 消除多冲的一步的影响
    mov cx, word ptr ds:[hex_len]
    mov si, offset hex;
    ;对比ds:si 与es:di cx步
    repe cmpsb
    je found
    pop di
    pop cx
    jmp in_buf_search

found:
    lea dx, fndalert
    mov ah, 09h
    int 21h
    pop di
    pop cx
    mov eax, dword ptr ds:[last_processed_len] ;已读长度
    add eax, dword ptr ds:[buflen] ;加上预计的buflen，然后使用cx减
    and ecx, 0ffffh  ;取低16位cx
    sub eax, ecx
    sub eax, 1  ;找到以后会有1bit冲过头，这里减掉
    mov dword ptr ds:[dest], eax
    call hexopt  ;输出十六进制数
    jmp exit

not_found:
    lea dx, nfndalert
    mov ah, 09h
    int 21h
    jmp exit

error:
    lea dx,failalert
    mov ah, 09h
    int 21h;

exit:
    mov ah, 4ch
    int 21h

code ENDS
end main
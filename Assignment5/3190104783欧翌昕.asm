.386
data SEGMENT USE16
    filename db 100, ?, 100 dup(0) ;存储打开文件的名称
    s db 100, ?, 100 dup(0);待查的内容输入字符串\
    hex db 100 dup(0);s 每两个转化为一个hex
    qlen db 4 dup(0);用来存一共query了几个二进制
    dest dd 1 dup(0); 查到所在的地址存在32bit中
    buf db 80 dup(0); 文件读取Buf
    buflen db 80, 0, 0, 0;buf长度，记得和上面的buf保持一样长
    fp dw 0  ;文件指针
    n dw 2 dup(0)  ;文件长度
    rn dw 2 dup(0);这个周期的已读文件总长
    oldn dw 2 dup(0);上周期的已读文件总长，方便最后确定位置
    hexTable db "0123456789ABCDEF"
    failalert db "Open file faile!$"
    fndalert db "found at $ "
    nfndalert db "Not found!$"
data ENDS

code SEGMENT USE16
ASSUME CS:CODE, DS:DATA

;32bit数输出，存在dest中
hexopt:
    push eax
    push bx
    push cx
    push dx
    push si
    mov eax, ds:[dest]
    mov bx, offset hexTable
    mov si, 1ch;

hexopt_iter_begin:
    push eax
    mov cx, si
    shr eax, cl
    and eax, 0fh;
    xlat
    mov ah, 02h
    mov dl, al
    int 21h
    pop eax
    cmp si, 00h;
    je hexopt_iter_end
    sub si, 04h
    jmp hexopt_iter_begin

hexopt_iter_end:
    pop si
    pop dx
    pop cx
    pop bx
    pop eax
    ret

;将一个char 0-9 A-F 转换为8bit
transfer:
    ;ds:di 为目标 读两位  结果8bit存到bh, bl为暂时的寄存器
    push dx;dx用来存有几位
    push cx;
    push di
    push si
    mov di, offset s + 2
    mov si, offset hex
    mov dl, 00h;

next_query:
    mov cx, 01h
    mov bx, 0

char2bx_begin:
    mov bl, ds:[di]
    ;结果存到bx
    cmp bl, '9'
    jbe is_num

is_alpha:
    sub bl, 'A'
    add bl, 0ah;
    jmp is_finish

is_num:
    sub bl, '0'

is_finish:
    inc di
    cmp cx, 0h;
    je char2bx_end
    dec cx
    mov bh, bl
    shl bh, 04h; 左移4位
    jmp char2bx_begin
    
char2bx_end:
    add bh, bl
    mov ds:[si], bh;
    inc si;
    inc dl;读取的总长加一
    cmp byte ptr ds:[di], 0dh;判断是否结束
    je query_end
    inc di;跳过空格
    jmp next_query

query_end:
    ; mov ds:[si], 0dh
    ;上面本来结束符，但是无法区分结束符和十六进制，所以用dl的总长来确定输入长度
    mov ds:[qlen], dl;
    pop si
    pop di
    pop cx
    pop dx
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
    int 21h
    mov dl, 0ah
    int 21h
    lea si, filename
    mov di, ds:[si+1];定位到filename长度
    and di, 0ffh;取后8 bit
    add di, si
    mov byte ptr ds:[di + 2], 00h;

    ;读入待查询字符串
    lea dx, s
    mov ah, 0ah  ;int 21h的0ah号功能，将字符串读取到ds:[dx]
    int 21h
    mov ah, 02h
    mov dl, 0dh
    int 21h
    mov dl, 0ah
    int 21h
    
    call transfer  ;处理s 存到 hex里

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

    ;保存文件长度到n 这里注意小端规则
    mov word ptr n[0], ax
    mov word ptr n[2], dx
    
    ;移动指针到文件开端
    mov ah, 42h
    mov al, 0
    mov bx, [fp]
    xor cx, cx
    xor dx, dx
    int 21h

    ;文件长度在n中
    ;rn中存已读完的长度, 这里初始化
    mov dword ptr ds:[rn], 00h;
    mov dword ptr ds:[oldn], 00h;

next_search_buf:
    ; 汇编语言读文件步骤:
    mov ebx, dword ptr ds:[n]
    mov eax, dword ptr ds:[rn]
    mov dword ptr ds:[oldn], eax;存一下上个周期的，方便最后定位找到的地方
    cmp ebx, eax
    je no_match;读完文件还没找到
    ;接下来在设置read_len
    add eax, dword ptr ds:[buflen]
    cmp eax, ebx
    jg buf_overflow
    ;否则正常取cx
    mov cx, word ptr ds:[buflen]
    sub cx, word ptr ds:[qlen]
    and ecx, 0ffffh;取后16bit 即cx
    add dword ptr ds:[rn], ecx;更新已读 减少qlen是为了保证连续的qlen个目标不会因为被截断而搜不到
    jmp set_buf_len_done
buf_overflow:
    mov eax, dword ptr ds:[rn]
    sub ebx, eax
    mov cx, bx;这时cx为剩下的长度
    mov ebx, dword ptr ds:[n]
    mov dword ptr ds:[rn], ebx;更新读完了
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
    mov cx, word ptr ds:[qlen]
    mov si, offset hex;
    ;对比ds:si 与es:di cx步
    repe cmpsb
    je match
    pop di
    pop cx
    jmp in_buf_search

match:
    lea dx, fndalert
    mov ah, 09h
    int 21h;
    pop di;
    pop cx
    mov eax, dword ptr ds:[oldn] ;已读长度
    add eax, dword ptr ds:[buflen] ;加上预计的buflen，然后使用cx减
    and ecx, 0ffffh;取cx
    sub eax, ecx
    sub eax, 01h; 找到以后会有1bit冲过头，这里减掉
    mov dword ptr ds:[dest], eax;
    call hexopt;输出十六进制数
    jmp exit
no_match:
    ; AH = 09h
    ; DS:DX -> '$'-terminated string
    ; 输出Not found
    lea dx, nfndalert
    mov ah, 09h
    int 21h;
    jmp exit

error:
    lea dx,failalert
    mov ah, 09h
    int 21h;

exit:
    mov ah, 4ch;
    int 21h

code ENDS
end main
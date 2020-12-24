.386
data segment use16
    buf db 6, ?, 6 dup(0) ;字符串缓冲区
	exp db 15 dup(0)
	deci db 15 dup(0) ;十进制结果字符串
	hex db 20 dup(0) ;十六进制结果字符串
	bin db 40 dup(0) ;二进制结果字符串
data ends

code segment use16
assume cs:code, ds:data

printToExp:
	nextNum:
	mov al, buf[si]
	cmp al, 0Dh
	jz break1
	mov exp[di], al
	inc si
	inc di
	jmp nextNum

	break1:
	ret

transToNum:
	push si
	mov ax, 0
	mov bx, 0
	mov si, 2

	again:
	mov bl, buf[si]
	cmp bl, 0Dh
	jz break2
	sub bl, '0'
	imul ax, 10
	add ax, bx
	inc si
	jmp again

	break2:
	pop si
	ret

printDeci:
	push ax
	push dx

	mov cx, 0
	mov si, 0
	nextNum2:
	push cx
	mov cx, 10
	push ax
	mov ax, dx
	mov dx, 0
	div cx   ;dx/10
	mov bx, ax  ;商高16位存放在bx
	pop ax
	div cx   ;ax/10,商低16位存放在ax
	pop cx

	push dx    ;将余数压栈
	mov dx, bx
	inc cx   ;压栈次数
	
	push dx
	add dx, ax
	cmp dx, 0  ;dx和ax均为0时结束循环
	pop dx
	jne nextNum2

	pop_again:  ;将栈中全部余数倒序存放到deci中
	pop dx
	add dl, '0'
	mov deci[si], dl
	inc si
	dec cx   
	jnz pop_again
	
	mov deci[si], '$'
	mov ah, 09h
	mov dx, offset deci
	int 21h

	pop dx
	pop ax
	ret

printHex:
	mov si, 0
	mov cx, 2
	
	loop1:
	push cx
	mov cx, 4
		
		loop2:
		push cx
		mov cx, 4
		rol dx, cl  ;dx左移4位
		pop cx
		
		push dx
		and dx, 0Fh
		cmp dx, 10
		jb isDigit		
		
		isAlpha:
		add dl, 'A'-10
		jmp next
		isDigit:
		add dl, '0'
		
		next:
		mov hex[si], dl
		pop dx
		inc si
		dec cx
		jnz loop2
	
	pop cx
	push dx
	mov dx, ax
	dec cx
	jnz loop1

	mov hex[si], 'h'
	inc si
	mov hex[si], '$'

	pop bx
	pop dx
	
	push ax
	push dx
	mov ah, 09h
	mov dx, offset hex
	int 21h
	pop dx
	pop ax
	ret

printBin:
	mov si, 0
	mov cx, 2
	
	loop1b:
	push cx
	mov cx, 4
		
		loop2b:
		push cx
		mov cx, 4
			
			loop3b:
			rol dx, 1
			mov bx, dx
			and bl, 01h
			add bl, '0'
			mov bin[si], bl
			inc si
			dec cx
			jnz loop3b
		
		pop cx
		mov bin[si], ' ' ;每隔4位空一格
		inc si
		dec cx
		jnz loop2b
	
	pop cx
	mov dx, ax
	dec cx
	jnz loop1b

	dec si
	mov bin[si], 'B'
	inc si
	mov bin[si], '$'

	mov ah, 09h
	mov dx, offset bin
	int 21h
	ret

endline:   ;换行
	push ax
	push dx
	mov dl, 0Dh
	mov ah, 02h
	int 21h
	mov dl, 0Ah
	mov ah, 02h
	int 21h
	pop dx
	pop ax
	ret

main:
    mov ax, data
    mov ds, ax

    lea dx, buf  ;buf用于存储字符串
    mov ah, 0Ah  ;int 21h的0Ah号功能，将字符串读取到ds:[dx]
    int 21h

	mov si, 2
	mov di, 0
	call printToExp

	mov exp[di], '*'
	inc di

	call transToNum
	push ax ;将第一个数压入栈中
	call endline

	lea dx, buf  ;buf用于存储字符串
    mov ah, 0Ah  ;int 21h的0Ah号功能，将字符串读取到ds:[dx]
    int 21h

	mov si, 2
	call printToExp
	mov exp[di], '='
	inc di
	mov exp[di], '$'

	call transToNum
	pop cx ;将第一个数弹出到cx中
	mul cx ;ax*cx,结果存放在dx和ax中
	call endline

	push dx
	push ax
	mov ah, 09h
	mov dx, offset exp
	int 21h ;输出计算表达式
	call endline
	pop ax
	pop dx

	call printDeci
	call endline

	call printHex
	call endline

	call printBin
	call endline

    mov ah, 4Ch
    int 21h  

code ends
end main
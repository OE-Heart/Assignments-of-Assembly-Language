.386
data segment use16
    buf db 6, ?, 6 dup(0) ;�ַ���������
	exp db 15 dup(0)
	deci db 15 dup(0) ;ʮ���ƽ���ַ���
	hex db 20 dup(0) ;ʮ�����ƽ���ַ���
	bin db 40 dup(0) ;�����ƽ���ַ���
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
	mov bx, ax  ;�̸�16λ�����bx
	pop ax
	div cx   ;ax/10,�̵�16λ�����ax
	pop cx

	push dx    ;������ѹջ
	mov dx, bx
	inc cx   ;ѹջ����
	
	push dx
	add dx, ax
	cmp dx, 0  ;dx��ax��Ϊ0ʱ����ѭ��
	pop dx
	jne nextNum2

	pop_again:  ;��ջ��ȫ�����������ŵ�deci��
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
		rol dx, cl  ;dx����4λ
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
		mov bin[si], ' ' ;ÿ��4λ��һ��
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

endline:   ;����
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

    lea dx, buf  ;buf���ڴ洢�ַ���
    mov ah, 0Ah  ;int 21h��0Ah�Ź��ܣ����ַ�����ȡ��ds:[dx]
    int 21h

	mov si, 2
	mov di, 0
	call printToExp

	mov exp[di], '*'
	inc di

	call transToNum
	push ax ;����һ����ѹ��ջ��
	call endline

	lea dx, buf  ;buf���ڴ洢�ַ���
    mov ah, 0Ah  ;int 21h��0Ah�Ź��ܣ����ַ�����ȡ��ds:[dx]
    int 21h

	mov si, 2
	call printToExp
	mov exp[di], '='
	inc di
	mov exp[di], '$'

	call transToNum
	pop cx ;����һ����������cx��
	mul cx ;ax*cx,��������dx��ax��
	call endline

	push dx
	push ax
	mov ah, 09h
	mov dx, offset exp
	int 21h ;���������ʽ
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
; libasm - partial implementation of libc in tasm
; uses CDECL as calling convention
; Copyright: 2016, Wasylews, S.Habor
; License: MIT


; int strlen(char *s) -> ax
strlen proc c
    uses bx
    arg @s

    mov bx, @s
    xor ax, ax

@@count_loop:
    cmp byte ptr[bx], 0
    je @@end_loop

    inc ax
    inc bx
    jmp @@count_loop

@@end_loop:
    ret
strlen endp


; int strcmp(char *s1, char *s2) -> ax
strcmp proc c
    arg @s1, @s2
    uses bx, si, di

    mov si, @s1
    mov di, @s2

@@strcmp_loop:
    mov al, [si]
    mov bl, [di]

    sub al, bl
    jne @@strcmp_done         ; return if different
    cmp bl, 0                 ; on end of both strings, ax = 0
    je @@strcmp_done
    inc si
    inc di
    jmp @@strcmp_loop

@@strcmp_done:
    cbw                     ; extend al to ax
    ret
strcmp endp


;int strncmp(const char *s1, const char *s2, size_t n) -> ax
strncmp proc c
	arg @s1, @s2, @n
	uses bx, si, di
	push @n
	push @s2
	push @s1
	call memcmp
	add sp, 6
	ret
strncmp endp


; char *strcat(char *dest, const char *src)
strcat proc c
	arg @dest, @src
	uses ax, si, di
	mov si, @dest
	push si
	call strlen
	add sp, 2
	add si, ax
	mov di, @src
@@cycle:
	mov al, [di]
	cmp al, 0
	je @@end
	mov [si], al
	inc si
	inc di
	jmp @@cycle
@@end:
	mov byte ptr[si], 0
	ret
strcat endp


; char *strncat(char *dest, const char *src, size_t n)
strncat proc c
	arg @dest, @src, @n
	uses ax, si, di
	mov si, @dest
	push si
	call strlen
	add sp, 2
	add si, ax
	mov di, @src
	mov cx, @n
@@cycle:
	cmp cx, 0
	je @@end
	mov al, [di]
	mov [si], al
	inc si
	inc di
	dec cx
	jmp @@cycle
@@end:
	mov byte ptr[si], 0
	ret
strncat endp


; char *strcpy(char *dest, const char *src) *dest -> ax
strcpy proc c
	arg @destr, @src
	uses si, di
	mov si, @src
	mov di, @dest
@@cycle:
	mov al, [si]
	cmp al, 0
	je @@end
	mov [di], al
	inc si
	inc di
	jmp @@cycle
@@end:
	mov byte ptr[di], 0
	mov ax, @dest
	ret
strcpy endp


; char *strncpy(char *dest, const char *src, size_t n) *dest -> ax
strncpy proc c
	arg @dest, @src
	uses si, di
	push @n
	push @src
	push @dest
	call memcpy
	ret
strncpy endp


; char *strpbrk(const char *s, const char *accept) -> ax
strpbrk proc c
	arg @s, @accept
	uses si, di
	mov si, @s
@@cycle1:
	mov al, [si]
	cmp al, 0
	je @@not_found
	mov di, @accept
@@cycle2:
	mov ah, [di]
	inc di
	cmp ah, 0
	je @@ns
	cmp ah, al
	jne @@cycle2
	mov ax, si
	jmp @@end
@@ns:
	inc si
	jmp @@cycle1
@@not_found:
	xor ax, ax
@@end:
	ret
strpbrk endp


; char *strchr(const char *s, int c) -> ax
strchr proc c
	arg @s, @c
	uses bx, cx
	mov bx, @s
	mov cx, @c
	mov ah, cl
	xor cx, cx
@@cycle:
	mov al, [bx]
	cmp al, 0
	je @@not_found
	inc bx
	cmp al, ah
	jne @@cycle
	dec bx
	mov ax, bx
	jmp @@end
@@not_found:
	xor ax, ax
@@end:
	ret
strchr endp


;char *strrchr(const char *s, int c) -> ax
strrchr proc c
	arg @s, @c
	uses bx, si
	push @s
	call strlen
	add sp, 2
	mov si, ax
	mov bx, @s
	mov ax, @c
	mov ah, al
@@cycle:
	cmp si, -1
	je @@not_found
	mov al, [bx+si]
	dec si
	cmp al, ah
	jne @@cycle
	inc si
	mov ax, bx
	add ax, si
	jmp @@end
@@not_found:
	xor ax, ax
@@end:
	ret
strrchr endp


; void *memset(void *memptr, int val, size_t num )
memset proc c
	arg @memptr, @val, @num
	uses ax, bx, cx
	mov ax, @val
	mov bx, @memptr
	mov cx, @num
@@cycle:
	cmp cx, 0
	je @@end
	mov [bx], al
	inc bx
	dec cx
	jmp @@cycle
@@end:
	ret
memset endp


; void *memcpy(void *dest, const void *src, size_t n) *dest -> ax
memcpy proc c
	arg @dest, @src, @n
	uses cx, si, di
	mov cx, @n
	mov si, @src
	mov di, @dest
@@cycle:
	cmp cx, 0
	je @@end
	mov al, [si]
	mov [di], al
	inc si
	inc di
	dec cx
	jmp @@cycle
@@end:
	add sp, 4
	mov ax, @dest
	ret
memcpy endp


; void *memmove(void *dest, const void *src, size_t n)
memmove proc c
	arg @dest, @src, @n
	uses bx, cx
	mov cx, @n
	sub sp, cx
	mov bx, sp
	push cx
	push @src
	push bx
	call memcpy
	add sp, 4
	push bx
	push @dest
	call memcpy
	add sp, 6
	add sp, cx
	ret
memmove endp


; void *memchr(const char *s, char c, size_t n) -> ax
memchr proc c
	arg @s, @c, @n
	uses bx, cx
	mov bx, @s
	mov cx, @n
	mov ax, @c
	mov ah, al
@@cycle:
	cmp cx, 0
	je @@not_found
	mov al, [bx]
	inc bx
	dec cx
	cmp al, ah
	jne @@cycle
	dec bx
	mov ax, bx
	jmp @@end
@@not_found:
	xor ax, ax
@@end:
	ret
memchr endp


; int memcmp(const char *s1, const char *s2, size_t n) -> ax
memcmp proc c
	arg @s1, @s2, @n
	uses cx, si, di
	mov cx, @n
	mov si, @s1
	mov di, @s2
@@cycle:
    cmp cx, 0 
    je @@e
	mov al, [si]
	mov ah, [di]
	sub al, ah
	jne @@strcmp_done
    inc si
    inc di
	dec cx
    jmp @@cycle
@@e:
	xor ax, ax
@@strcmp_done:
    cbw                     ; extend al to ax
    ret	
	ret
memcmp endp


; char upper(char c) -> al
upper proc c
    arg @@c
    mov ax, @@c
    cmp al, 'a'
    jb @@end
    cmp al, 'z'
    ja @@end
    sub al, 32
@@end:
    ret
upper endp


; toupper(char *s)
toupper proc c
    uses bx, dx
    arg @s
    mov bx, @s
@@beg:
    mov dl, [bx]
    push dx
    call upper
    add sp, 2
    mov [bx], al
    inc bx
    cmp byte ptr[bx], 0
    jne @@beg
    ret
toupper endp


; char lower(char c) -> al
lower proc c
    arg @@c
    mov ax, @@c
    cmp al, 'A'
    jb @@end
    cmp al, 'Z'
    ja @@end
    add al, 32
@@end:
    ret
lower endp


; tolower(char *s)
tolower proc c
    uses bx, dx
    arg @s
    mov bx, @s
@@beg:
    mov dl, [bx]
    push dx
    call lower
    add sp, 2
    mov [bx], al
    inc bx
    cmp byte ptr[bx], 0
    jne @@beg
    ret
tolower endp
GLOBAL syscallHandler
EXTERN loadSysNum
EXTERN syscallDispatcher

SECTION .text

; en rax viene el numero de syscall a ejecutar
syscallHandler:
	pushState
  push rdi
  mov rdi, rax
  call loadSysNum
  pop rdi
  call syscallDispatcher
	popState
	iretq            ; ret especifico para volver de interrupciones    

%macro pushState 0
	; push rax 			 ; no pusheamos rax porque se usa luego para retornar en la syscall
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	; pop rax
%endmacro

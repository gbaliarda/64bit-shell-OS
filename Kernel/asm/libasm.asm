GLOBAL cpuVendor
GLOBAL initProcess

GLOBAL runScheduler

GLOBAL _xchg

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

initProcess:
	push rbp
	push rsp
	mov rax, rsp
	
	mov rsp, rdi
	sub rdi, 160 ; deja el rsp al final del "stackframe de int", para que luego se haga popstate + iretq y se popee en el orden correcto
	mov rbp, rdi

	; <-rsp
	push qword 0x0   ; ss
	push qword rsp   ; rsp
	push qword 0x202 ; rflags
	push qword 0x8   ; cs
	push qword rsi   ; rip
	; registros generales
	push qword 0x1
	push qword 0x2
	push qword 0x3
	push qword 0x4
	push qword 0x5
	push qword rdx ; argc
	push qword rcx ; argv
	push qword 0x6
	push qword 0x7
	push qword 0x8
	push qword 0x9
	push qword 0xA
	push qword 0xB
	push qword 0xC
	push qword 0xD

	mov rsp, rax
	mov rax, rbp
	pop rsp
	pop rbp
	ret

runScheduler:	
	sti
	int 20h


_xchg:
	mov rax, rsi 
	xchg [rdi], eax
	ret

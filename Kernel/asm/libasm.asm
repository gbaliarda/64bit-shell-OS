GLOBAL cpuVendor
GLOBAL initFirstProcess

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

initFirstProcess:
	push rbp
	push rsp
	mov rax, rsp
	
	mov rsp, rdi
	sub rdi, 160 ; deja el rsp al final del "stackframe de int", para que luego se haga popstate + iretq y se popee en el orden correcto
	mov rbp, rdi

	push qword 0x0
	push qword rbp
	push qword 0x202
	push qword 0x8
	push qword 0x400000
	; registros
	push qword 0x0 ; argc
	push qword 0x0 ; argv
	push qword 0x1
	push qword 0x2
	push qword 0x3
	push qword 0x4
	push qword 0x5
	push qword 0x6
	push qword 0x7
	push qword 0x8
	push qword 0x9
	push qword 0xA
	push qword 0xB
	push qword 0xC
	push qword 0xD

	mov rsp, rax
	pop rsp
	pop rbp
	ret

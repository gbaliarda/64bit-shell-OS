GLOBAL saveState

section .text


%macro saveRegister 1
	mov rax, [rsp + %1]
	mov [rdi], rax
	add rdi, 8
%endmacro

saveState:
	saveRegister 136  ; rax
	saveRegister 128
	saveRegister 120
	saveRegister 112
	saveRegister 104
	saveRegister 96
	saveRegister 88
	saveRegister 80
	saveRegister 72
	saveRegister 64
	saveRegister 56
	saveRegister 48
	saveRegister 40
	saveRegister 32
	saveRegister 24		; r15
  ret
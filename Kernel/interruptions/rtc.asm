GLOBAL getTimeRTC

SECTION .text
getRTC:
	push rbp
	mov rbp, rsp
	push rbx
	mov rbx, [rbp+24]		

	xor rax, rax

	mov al, [rbp+16]		
	out 70h, al				
	in al, 71h          	; Obtengo las horas/minutos/segundos según [rbp+16] 
	call BCDtoDec

	mov [rbx], eax	
	pop rbx
	leave
	ret


BCDtoDec:
	mov dl, al
	mov cl, dl
	shr cl, 4			
	mov al, 10		 
	mul cl				 
	and dl, 0Fh		 
	add dl, al		 
	mov al, dl		 
	ret

%macro getData 1
	add rdi, 4
	push rdi
	push %1					
	call getRTC
	add rsp, 16
%endmacro

getTimeRTC:
	push rbp
	mov rbp, rsp

	sub rdi, 4 ; 

	getData 4 ; horas
	getData 2 ; minutos
	getData 0 ; segundos
	getData 7 ; dia
	getData 8 ; mes
	getData 9 ; año
	
	leave
	ret
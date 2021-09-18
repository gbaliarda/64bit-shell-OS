GLOBAL getTimeRTC

SECTION .text
getRTC:
	push rbp
	mov rbp, rsp
	push rbx
	mov rbx, [rbp+24]		; carga el puntero a la estructura

	xor rax, rax

	mov al, [rbp+16]		; parametro de que se quiere obtener
	out 70h, al				; escribo en la posicion 70h el parametro
	in al, 71h          	; Obtengo las horas/minutos/segundos según [rbp+16] 
	call BCDtoDec

	mov [rbx], eax	;guardo en la estructura el resultado, usando eax para que ocupe los 4bytes del int
	pop rbx
	leave
	ret

; Convierte un numero en formato BCD (binario codificado en decimal) el cual tiene las decenas en los higher 4 bits, y las unidades en los lower 4 bits. Recibe el BCD por al y lo retorna por ahi mismo.
BCDtoDec:
	mov dl, al
	mov cl, dl
	shr cl, 4			 ; shiftea cl 4 bits para la derecha (eliminando las unidades) y rellena con 0s a la derecha
	mov al, 10		 
	mul cl				 ; multiplico por 10 las decenas - ax = al * cl
	and dl, 0Fh		 ; me quedo con las unidades
	add dl, al		 ; formo el numero decimal listo para devolver (las decenas entran en al pues son numeros pequeños)
	mov al, dl		 ; guardo el resultado final en al para retornar
	ret

%macro getData 1
	add rdi, 4
	push rdi
	push %1					; dato a obtener
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
GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_inforeg
GLOBAL sys_printmem
GLOBAL sys_getDateTime
GLOBAL sys_clearScreen

GLOBAL sys_consoleInUse

section .text

sys_read:
  mov rax, 0
  int 80h
  ret
sys_write:
  mov rax, 1
  int 80h
  ret

sys_inforeg:
  mov rax, 10
  int 80h
  ret

sys_printmem:
  mov rax, 11
  int 80h
  ret

sys_getDateTime:
  mov rax, 12
  int 80h
  ret

sys_clearScreen:
  mov rax, 13
  int 80h
  ret

sys_consoleInUse:
  mov rax, 14
  int 80h
  ret

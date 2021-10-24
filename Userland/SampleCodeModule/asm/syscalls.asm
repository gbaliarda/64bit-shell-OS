GLOBAL sys_write
GLOBAL sys_read
GLOBAL sys_inforeg
GLOBAL sys_printmem
GLOBAL sys_getDateTime
GLOBAL sys_clearScreen
GLOBAL sys_createProcess
GLOBAL sys_exit
GLOBAL sys_printProcess
GLOBAL sys_killProcess
GLOBAL sys_changePriority
GLOBAL sys_changeState
GLOBAL sys_semOpen
GLOBAL sys_semClose
GLOBAL sys_semWait
GLOBAL sys_semPost
GLOBAL sys_printSemaphores

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

sys_alloc:
  mov rax, 14
  int 80h
  ret

sys_free:
  mov rax, 15
  int 80h
  ret

sys_memStatus:
  mov rax, 16
  int 80h
  ret

sys_createProcess:
  mov rax, 17
  int 80h
  ret

sys_exit:
  mov rax, 18
  int 80h
  ret

sys_printProcess:
  mov rax, 19
  int 80h
  ret

sys_killProcess:
  mov rax, 20
  int 80h
  ret

sys_changePriority:
  mov rax, 21
  int 80h
  ret

sys_changeState:
  mov rax, 22
  int 80h
  ret

sys_semOpen:
  mov rax, 23
  int 80h
  ret

sys_semClose:
  mov rax, 24
  int 80h
  ret

sys_semWait:
  mov rax, 25
  int 80h
  ret
  
sys_semPost:
  mov rax, 26
  int 80h
  ret

sys_printSemaphores:
  mov rax, 27
  int 80h
  ret
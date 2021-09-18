GLOBAL getKey

section .text

getKey:
    in al, 60h			; lee el output register
    ret

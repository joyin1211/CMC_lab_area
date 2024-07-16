section .data
    const3 dq 3.0
    constn1 dq -1.0
    consthlf dq 0.5
    ;myconst dq -1
    ;ft db "%.10lf", 10, 0

section .text
;extern printf, scanf
;global main
global _f1, _f2, _f3

_f1:
    push ebp
    mov ebp, esp
    
    finit
    fld qword [ebp+8] ; st0 = x
    fld1 ; st0 = 1.0, st1 = 0,5
    fsubp ;st0 = x - 1
    fld st0 ;st1 = x - 1, st0 = x - 1
    fmulp ;st0 = (x - 1) ^ 2
    fld1 ;st0 = 1, st1 = (x - 1) ^ 2
    faddp ; st0 = (x - 1) ^ 2  + 1
    fld qword [const3] ; st0 = 3.0, st1 = (x - 1) ^ 2  + 1
    fxch ; st1 = (x - 1) ^ 2  + 1, st0 = 3.0
    fdivp ; st0 = 3 / ((x - 1) ^ 2  + 1)

    leave
    ret

_f2:
    push ebp
    mov ebp, esp

    finit
    fld qword [ebp + 8]
    fadd qword[consthlf]
    fsqrt
    
    leave
    ret

_f3:
    push ebp
    mov ebp, esp
    finit
    fld qword[ebp + 8] ;st0 = x
    fldl2e ;st0 = log_2(e)
    fmulp ;st0 = x * log_2(e)
    fld st0 ;st1 = x * log_2(e)
    fld qword[constn1]
    fmulp st1
    fstp st1
    fld st0 ; 
    frndint ; st0 = [-x*log2e]
    fxch    ; st1 = -x*log2e, st1 = [-x*log2e]
    fsub st1 ; st0 = {-x*lgo2e}
    f2xm1   ; st0 = 2^{-x*log2e} - 1
    fld1    ; st0 = 1.0
    faddp   ; st0 = 2^{-x*log2e}
    fscale  ; st0 = 2^(-x*log2e)
    leave
    ret

;main:
;    mov ebp, esp; for correct debugging
;    xor eax, eax
;    
;   push dword[myconst + 4]
;   push dword[myconst]
;    call f2
;    add esp, 8
;    
;    fstp qword[myconst]
;    
;    ;print
;    push dword[myconst + 4]
;    push dword[myconst]
;    push ft
;    call printf
;   add esp, 12
;   xor eax, eax
;    ret

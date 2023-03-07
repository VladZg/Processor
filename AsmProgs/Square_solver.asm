push 1000
pop rdx

start:

pin
pop rax
pin
pop rbx
pin
pop rcx

push 0
push rax
je here

call quadratic_solver
jmp finish

here:
push rbx
pop rax
push rcx
pop rbx
call linear_solver

finish:

jmp start

hlt

quadratic_solver:
{
    push rbx
    push -1
    mul
    push rdx
    mul
    call discreminant
    push rdx
    push rdx
    mul
    mul
    square
    add
    push rax
    push 2
    mul
    div
    out_float

    push rbx
    push -1
    mul
    push rdx
    mul
    call discreminant
    push rdx
    push rdx
    mul
    mul
    square
    sub
    push rax
    push 2
    mul
    div
    out_float

    print 2
    ret
}

linear_solver:
{
    push rax
    push 0
    je inf_roots

    push rbx
    push -1
    mul
    push rdx
    mul
    push rax
    div
    out_float

    print 1
    ret
}

discreminant:
{
    push rbx
    push rbx
    mul
    push 4
    push rax
    push rcx
    mul
    mul
    sub
    pop rex

    push rex
    push 0
    jb no_roots

    push rex
    push 0
    je one_root

    push rex
    ret
}

no_roots:
{
    print 0
    jmp start
}

inf_roots:
{
    print 133722869
    jmp start
}

one_root:
{
    push rbx
    push -1
    mul
    push rdx
    mul
    push 2
    push rax
    mul
    div
    out_float

    print 1
    jmp start
}

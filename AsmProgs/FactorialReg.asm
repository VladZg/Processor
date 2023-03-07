jmp start

FACTORIAL (rbx, rcx)
{
    push -1 + rbx
    push 0
    jbe here

    push rbx
    push rcx
    mul
    pop rcx

    push -1 + rbx
    pop rbx

    call FACTORIAL

    here
    push rcx
    push rbx
    mul
    pop rcx

    Dump 80 105

    RET
}

start

pin
pop rbx

push rbx
push 0
je finish

push 1
pop rcx
call FACTORIAL
print rcx
dump full

jmp start

finish

hlt

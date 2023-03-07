main:
{
    push 0
    push 0
    pop rax
    pop rbx
    Pin
    Pop rbx
    start:
    Push 1 + rax
    Pop rax
    call square
    Out
    Push rbx
    Push rax
    ja start
    jmp main
    hlt
}

square:
{
    Push rax
    Push rax
    Mul
    Pop rcx
    Push rcx
    ret
}

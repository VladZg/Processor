jmp main

func
{
    push 0
    pop [rbx]
    dump 0 100
    RAM
    ret
}

main
{
    dump 0 100

    fill
    call func

    pin
    pop rbx

    push rbx
    push 0
    jne fill

    RUS

    hlt
}

RUS

jmp main

cleaner:
{
    push 0
    push 0
    push 0
    pop rax
    pop rbx
    pop rcx

    RET
}

fill_cell:
{
    pop rax
    push 0
    pop [rax]

    RET
}

fill_circle:
{
    push 1 + rbx
    pop rbx
    push 87266
    push rcx
    add
    pop rcx

    push 41
    COS rcx
    push rdx
    push 2
    mul
    mul
    push 10000000
    div
    add

    push 10
    SIN rcx
    push rdx
    mul
    push 10000000
    div
    sub

    push 81
    mul
    add
    call fill_cell

    push 1 + rbx
    push 720
    jbe fill_circle

    RET
}

draw_circle:
{
    call cleaner

    dump 0 100
    CLEAR_RAM
    call fill_circle
    CLEAR_CONSOLE
    RAM
    DELAY 100

    RET
}

cycle:
{
    push -1
    pop rdx

    start1:
    push 1 + rdx
    pop rdx
    call draw_circle
    push 1 + rdx
    push 10
    jbe start1

    start2:
    push -1 + rdx
    pop rdx
    call draw_circle
    push -1 + rdx
    push 0
    jae start2

    CLEAR_RAM

    RET
}

main:
{
    pin
    pop rfx

    start:
    push 1 + rex
    pop  rex
    call cycle
    push 1 + rex
    push rfx
    jbe start

    hlt
}

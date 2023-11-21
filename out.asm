global _start
_start:
        mov rax, 12
        push rax
        mov rax, 12
        push rax
        push QWORD [rsp + 8]
        push QWORD [rsp + 8]

        pop rax
        pop rbx
        cmp rax,rbx
        push rax
        jne L1
        push QWORD [rsp + 16]
        mov rax, 60
        pop rdi
        syscall
        add rsp,0
        jmp L2
L1:
        mov rax, 3
        push rax
        mov rax, 60
        pop rdi
        syscall
        add rsp,0

L2:
        mov rax, 2
        push rax
        mov rax, 60
        pop rdi
        syscall
        mov rax, 60
        mov rdi, 0

        syscall
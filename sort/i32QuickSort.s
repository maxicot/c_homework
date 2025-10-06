.intel_syntax noprefix

.text

.globl i32QuickSort

i32QuickSort:
    dec rsi
    mov rdx, rsi
    xor rsi, rsi
_sort:
    push r14
    push r13
    xor r14, r14
    movsx rax, edx
    push r12
    mov r13, rax
    lea r12, [rdi + rax * 4]
    push rbp
    mov rbp, rdi
    push rbx
    push rcx
_if:
    cmp esi, r13d
    jge _return
    mov edi, DWORD PTR [r12]
    movsx rax, esi
    mov ebx, esi
_swap1:
    movsx rdx, ebx
    lea rdx, [rbp + rdx * 4]
    cmp r13d, eax
    jle _swap2
    mov ecx, DWORD PTR [rbp + rax * 4]
    cmp ecx, edi
    jg _for
    mov r8d, DWORD PTR [rdx]
    mov DWORD PTR [rdx], ecx
    inc ebx
    mov DWORD PTR [rbp + rax * 4], r8d
    inc r14
_for:
    inc rax
    jmp _swap1
_swap2:
    mov eax, DWORD PTR [rdx]
    mov ecx, DWORD PTR [r12]
    mov rdi, rbp
    mov DWORD PTR [rdx], ecx
    lea edx, [rbx - 1]
    mov DWORD PTR [r12], eax
    inc r14
    call _sort
    lea esi, [rbx + 1]
    jmp _if
_return:
    pop rax
    mov rax, r14
    pop rbx
    pop rbp
    pop r12
    pop r13
    pop r14
    ret

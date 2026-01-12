.intel_syntax noprefix
.global _start

.text

_start:
    xor rbp, rbp      # mark end of stackframes

    mov rdi, [rsp]    # C expects argc in rdi
    lea rsi, [rsp+8]  # C expects argv in rsi
    and rsp, -16      # align stack to 16 bytes
    call main
    
    mov rdi, rax      # main returns with exit code in rax
    mov rax, 60       # exit syscall code = 60
    syscall

.intel_syntax noprefix
.global _linux_syscall

.text

_linux_syscall:
    # prologue
    push rbp          # save caller stackframe
    mov rbp, rsp      # set up my stackframe

    # C Caller provides:    RDI, RSI, RDX, RCX, R8, R9, Stack
    # Linux Kernel expects: RDI, RSI, RDX, R10, R8, R9
    mov r10, rcx      # C arg3 -> linux arg3
    mov rax, [rbp+16] # C arg6 -> linux syscall id
    syscall

    # epilogue
    mov rsp, rbp      # clear my stackframe
    pop rbp           # restore caller stackframe
    ret

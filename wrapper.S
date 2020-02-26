/*
 * wrapper.S - syscall wrappers declarations
 */

#include <asm.h>

ENTRY(write)
  pushl %ebp
  movl %esp, %ebp
  pushl %ebx
  movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
  movl 16(%ebp), %edx
  movl $4, %eax
  int $0x80
  cmpl $0, %eax
  jge no_error
  negl %eax
  movl %eax, errno
  movl $-1, %eax
no_error:
  popl %ebx
  popl %ebp
  ret
   


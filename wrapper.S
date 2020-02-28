/*
 * wrapper.S - syscall wrappers declarations
 */

#include <asm.h>

ENTRY(write) //exemple usant int
	//enllaç dinàmic
  pushl %ebp
  movl %esp, %ebp
  pushl %ebx
  //paràmetres
	movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
  movl 16(%ebp), %edx
  movl $4, %eax
  //crida
	int $0x80
  cmpl $0, %eax
  jge no_error_write
  negl %eax
  movl %eax, errno
  movl $-1, %eax
no_error_write:
  popl %ebx
  popl %ebp
  ret
   
ENTRY(gettime) //exemple usant systenter
	pushl %ebp
	movl %esp, %ebp
	//identificador
	movl $10, %eax
	//empilar registres pel retorn
	pushl %ecx
	pushl %edx
	//guardem retorn
	pushl $return
	//nou enllaç dinamic
	push %ebp
	movl %esp, %ebp
	//crida
	sysenter 
return:
	//restaurem enllaç dinamici estat de la pila
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
  jge no_error_gettime
  negl %eax
  movl %eax, errno
  movl $-1, %eax
no_error_gettime:
  popl %ebp
  ret


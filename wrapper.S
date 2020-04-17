/*
 * wrapper.S - syscall wrappers declarations
 */

#include <asm.h>
#define SYSCALL(syscalln) \
	leal syscall_ret##syscalln, %eax; \
	push %eax; \
	movl $syscalln, %eax; \
	push %ebp; \
	movl %esp, %ebp; \
	sysenter; \
	syscall_ret##syscalln: pop %ebp; \
	addl $4, %esp


#define CHECK_ERROR(end) \
	cmpl $0, %eax; \
	jge  end; \
	negl %eax; \
	leal errno, %ebx; \
	movl %eax, (%ebx); \
	movl $-1, %eax



ENTRY(write) //exemple usant int
	push %ebp
	movl %esp, %ebp
	push %ebx
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx
	SYSCALL(4);
	CHECK_ERROR(write.no_error);
	write.no_error:
	pop %ebx
	pop %ebp
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

ENTRY(getpid)
	pushl %ebp
	movl %esp, %ebp
	//identificador de la syscall
	movl $20, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	//guardem la etiqueta de retorn
	push $return_getpid
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_getpid:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_getpid //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_getpid:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret


ENTRY(fork)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $2, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	//guardem la etiqueta de retorn
	push $return_fork
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_fork:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_fork //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_fork:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret

ENTRY(exit)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $1, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
	//guardem la etiqueta de retorn
	push $return_exit
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_exit:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	popl %ebp
	ret


ENTRY(get_stats)
	pushl %ebp
	movl %esp, %ebp
	//syscall ID
	movl $35, %eax
	//empilar registres pel retorn (aquests cal guardarlos quan es fa un sysenter)
	pushl %ecx
	pushl %edx
//paràmetres
	movl 8(%ebp), %ebx
  movl 12(%ebp), %ecx
	//guardem la etiqueta de retorn
	push $return_getstats
	//nou enllacç dinamic
	pushl %ebp
	movl %esp, %ebp
	sysenter
return_getstats:
	//restaurem enllaç dinamic
	popl %ebp
	//elimino l'adreça de retorn
	addl $4, %esp
	//restauro registres
	popl %edx
	popl %ecx
	//comporovo que no hi hagi errors
	cmpl $0, %eax
	jge no_error_getstats //salto a la resta de la rutina
	//tracto l'error
	negl %eax
	movl %eax, errno
	movl $-1, %eax
no_error_getstats:    //he d'execurtar sempre aixo en quaslevol cas per sortir de la rutina.  
	popl %ebp
	ret



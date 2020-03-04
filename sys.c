/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <errno.h>  //l'hem afegit nosaltres a la carpeta d'includes

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <libc.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}


int sys_write(int fd, char * buffer, int size)
{
  
	int fd_ret;
  // Check user
  if( (fd_ret = check_fd(fd, ESCRIPTURA)) > 0) return fd_ret;
 
  if(buffer == NULL) return -EFAULT; // EFAULT - buf is outside your accessible address space
  if(size < 0) return -EDOM; //No comprovem el tamany del buffer i això pot causar que acabem printant tota la pila sia  l'usuari li ve de guust. Pot ser un problema de seguretat. Com ho fem? Implementem aquí un strlen?
	int chunk_size = 500;
	char dest[chunk_size];
	int i, ret;
	int limit = size/chunk_size;
	int total = 0;
	for(i = 0; i < limit; i+=chunk_size)
	{
		copy_from_user(&buffer[i] ,dest, chunk_size);
		if((ret = sys_write_console(dest, chunk_size )) > 0 ) total += ret;
		else return ret;
	}
	
		copy_from_user(&buffer[i] ,dest, size%chunk_size);
		if((ret = sys_write_console(dest, size%chunk_size )) > 0 ) total += ret;
		else return ret;
		
	
  return total;
}

int sys_gettime(){
	return zeos_ticks;
}

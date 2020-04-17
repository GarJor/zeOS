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

extern void init_stat(struct task_struct *t);
extern struct task_struct * get_task(int pid);
extern int zeos_ticks;
extern struct list_head  freequeue, readyqueue;
int nextPID = 2; // COM? i ON?
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

int ret_from_fork()
{
	return 0;
}

int sys_fork()
{
  int PID=-1;

  // a) creates the child process
	if(list_empty( &freequeue )) return -EAGAIN;
 	struct list_head * free_head = list_first( &freequeue );
	list_del(free_head);
	struct task_struct * child_task = list_head_to_task_struct(free_head); 
	
	// b) copiem data del pare al fill
	copy_data(current(), child_task, sizeof(union task_union));

	// c) inicialitzem el directori
	allocate_DIR(child_task);

	// d) cerquem pagines fisiques per mapejar amb les logiques
	int frames[NUM_PAG_DATA];
	int i;
	for (i=0; i<NUM_PAG_DATA; i++) 
	{
		frames[i] = alloc_frame();
		if(frames[i] < 0) 
		{ 																								 // En cas d'error:
			for(int j = 0; j < i; j++) free_frame(frames[j]); // alliberem els frames reservats
			list_add(&child_task->list, &freequeue);				 // tornem a encuar la tasca al freequeue 			
			return -ENOMEM;
		}

	}
	// e) heredem les dades del pare
	page_table_entry *parent_PT = get_PT(current());
	page_table_entry *child_PT = get_PT(child_task);

	//  > i) creacio de l'espai d'adresses del process fill: 
	

	// 	>> A) compartim codi de sistema i usuari
	for(i=1; i < NUM_PAG_KERNEL; i++)  // suposem que el codi de sistema esta mapejat a les primeres posicions de la taula de pagines
	{
		child_PT[1+i].entry = parent_PT[1+i].entry;
	}

	for(i=0; i < NUM_PAG_CODE; i++) 
	{
		set_ss_pag(child_PT, i+PAG_LOG_INIT_CODE, get_frame(parent_PT, i+PAG_LOG_INIT_CODE));

	}
	
	// 	>> B) assignem les noves pagines fisiques a les adresses logiques del proces fill 
	for(i=0; i < NUM_PAG_DATA; i++) 
	{
		set_ss_pag(child_PT, i+PAG_LOG_INIT_DATA, frames[i]);

	}

	// > ii) ampliem lespai d'adresses del pare per poder accedir a les pagines del fill per copiar data+stack
	for(i=0; i < NUM_PAG_DATA; i++) 
	{
		unsigned int page = i+PAG_LOG_INIT_DATA;
		set_ss_pag(parent_PT, page+NUM_PAG_DATA, frames[i]);
		copy_data( (unsigned long *)(page*PAGE_SIZE), (unsigned long *)((page+NUM_PAG_DATA)*PAGE_SIZE) , PAGE_SIZE);
		del_ss_pag(parent_PT, page+NUM_PAG_DATA);
		

	}
	set_cr3(get_DIR(current())); // flush de la TLB	


	// f) assignem nou PID
	PID = nextPID++;
	child_task->PID = PID;
	
	// g) modifiquem els camps que han de canviar en el proces fill
	init_stat(child_task);


	// h) preparem la pila del fill per al task_switch
	union task_union * child_union = (union task_union *) child_task;
	
	((unsigned long *)KERNEL_ESP(child_union))[-0x13] = (unsigned long) 0;
	((unsigned long *)KERNEL_ESP(child_union))[-0x12] = (unsigned long) ret_from_fork; // 5 registres + SAVE_ALL (11 regs) + @handler -> top de stack a -0x11
	child_task->kernel_esp = 	&((unsigned long *)KERNEL_ESP(child_union))[-0x13]; 

	// i) empilem a la readyqueue el fill
	list_add_tail(&child_task->list, &readyqueue);

	// j) retornem el PID del fill


	return PID;
}

void sys_exit()
{  
	struct task_struct *task = current();
	page_table_entry *task_PT = get_PT(task);
	for(int i =0; i < NUM_PAG_DATA ; ++i)
	{
		free_frame(get_frame(task_PT,i+PAG_LOG_INIT_DATA));
		del_ss_pag(task_PT, i+PAG_LOG_INIT_DATA);
	}
	
	task->PID=-1;
	update_process_state_rr(task, &freequeue);
	sched_next_rr();
	
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

int sys_get_stats(int pid, struct stats *st){
	if(!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT;
    if(pid < 0) return -EINVAL;
    struct task_struct *task = get_task(pid);
    if (task != 0) {
        struct stats * task_st = &(task->estat);
        copy_to_user(task_st, st, sizeof(struct stats));
    }
    return task == 0 ? -ESRCH : 0;

}

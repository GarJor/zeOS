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

#include <circular_buffer.h>



#define LECTURA 0
#define ESCRIPTURA 1

extern void init_stat(struct task_struct *t);
extern struct task_struct * get_task(int pid);
extern int zeos_ticks;
extern struct circular_buffer keyboard_buffer;
extern int quantum_ticks;
extern int spf_sem;
extern int	gfps;
extern int	spf_ticks;
extern struct list_head  freequeue, readyqueue, priorityqueue;
extern char *spf_last_screen;
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
	
	// b) copiem pcb del pare al fill
	copy_data(current(), child_task, sizeof(union task_union));

	// c) inicialitzem el directori
	allocate_DIR(child_task);

	// d) cerquem pagines fisiques per mapejar amb les logiques
	int brk = (int) current()->brk;
	int NUM_PAG_DATA_HEAP = NUM_PAG_DATA + (brk & 0x0fff)? PAG_HEAP(brk) : PAG_HEAP(brk) - 1; 
	int frames[NUM_PAG_DATA_HEAP];   //cal afegir les pags del heap
	int i;
	for (i=0; i<NUM_PAG_DATA_HEAP; i++) 
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
	for(i=0; i < NUM_PAG_DATA_HEAP; i++) 
	{
		set_ss_pag(child_PT, i+PAG_LOG_INIT_DATA, frames[i]);

	}

	// > ii) ampliem lespai d'adresses del pare per poder accedir a les pagines del fill per copiar data+stack
	for(i=0; i < NUM_PAG_DATA_HEAP; i++) 
	{
		unsigned int page = i+PAG_LOG_INIT_DATA;
		set_ss_pag(parent_PT, page+NUM_PAG_DATA_HEAP, frames[i]);
		copy_data( (unsigned long *)(page*PAGE_SIZE), (unsigned long *)((page+NUM_PAG_DATA_HEAP)*PAGE_SIZE) , PAGE_SIZE);
		del_ss_pag(parent_PT, page+NUM_PAG_DATA_HEAP);
		

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
	if (child_task->nice)list_add_tail(&child_task->list, &readyqueue);
	else list_add_tail(&child_task->list, &priorityqueue);

	// j) retornem el PID del fill


	return PID;
}

void sys_exit()
{  
	struct task_struct *task = current();
	page_table_entry *task_PT = get_PT(task);
	int brk = (int) current()->brk;
	int NUM_PAG_DATA_HEAP = NUM_PAG_DATA + (brk & 0x0fff)? PAG_HEAP(brk) : PAG_HEAP(brk) - 1; 
	for(int i =0; i < NUM_PAG_DATA_HEAP ; ++i)
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
				task_st->remaining_ticks = quantum_ticks;  //Cal actualitzar els ticks!!
        copy_to_user(task_st, st, sizeof(struct stats));
    }
    return task == 0 ? -ESRCH : 0;

}

int sys_get_key(char *c) {
	if (c == NULL) return NULL;
	char aux;
	int ret = circular_read(&keyboard_buffer, &aux);
	copy_to_user(&aux, c, 1);
 	return ret;
}

int sys_put_screen(char * s){		
	if (s == NULL) return 0;
	if(spf_sem > 0 || spf_ticks == -1){	
		 print_screen(s);	
		--spf_sem;
		spf_last_screen = NULL;
		return 1;
	}
	spf_last_screen = s;
	return 0;
}

int sys_set_fps(int fps) {
	if(fps == -1) {
		spf_ticks = -1;
		current()->nice = 1;
	} else {
		current()->nice = 0;
		gfps = fps;
		spf_ticks =18;
		spf_sem = fps;
	}
	return 1;
}

int sys_fflush() {
	circular_init(&keyboard_buffer);
	return 1;
}

//void* sys_sbrk(int n){
////mirar quantes pàgines hem de reservar
//	int brk = (int)current()->brk;
//	int current_page = ((brk & 0x0fff) == 0)? PAG_HEAP(brk)-1 : PAG_HEAP(brk); // la pagina on estic ha estat alocatada? 
//	int dst = (PAG_HEAP(brk+n) < 0)? L_HEAP_START : brk+n; // comprovem el parametre de usuari
//	int pags = PAG_HEAP(dst) - current_page; //quantes pagines reservarem
//	
//	if (pags > 0){
//		//mirar si hi ha espai o no?
//		int frames[pags];
//		int i;
//		for (i = 0; i < pags; i++){
//			frames[i] = alloc_frame();
//			if (frames[i]<0){
//				for (int j = 0; j < i; j++)
//					free_frame(frames[j]);
//				return (void*) -ENOMEM;
//			}	
//		}
//		page_table_entry *pt = get_PT(current()); 	
//		for (i = 1; i <= pags; i++){ 
//			set_ss_pag(pt, HEAP_FIRST_PAGE+current_page+i, frames[i-1]);
//		}
//	} else if (pags < 0){  //cal restart pags
//		//desalocatar les pagines 
//		int del_pags = (((dst) &  0x0fff) == 0)?  (-1*pags)+1 : -1*pags; // pagina final esta buida tamb, la alliberarem
//		page_table_entry *pt = get_PT(current()); 	
//		for(int i = 0;  i < del_pags; ++i) {
//			del_ss_pag(pt, HEAP_FIRST_PAGE+PAG_HEAP(brk)-i);
//		}
//							
//	}
//	current()->brk =  (void *)dst;
//	return (void *)brk; //el d'abans, el de sempre
//}

int inici_pag(unsigned long brk){     //mira si estic alineat a pàgina
	return (brk & 0x0fff) == 0;
}

void* sys_sbrk(int n){
	int ini_pag;
	page_table_entry *pt = get_PT(current());
	unsigned long brk = (unsigned long) current()->brk;
	if (brk == L_HEAP_START && n < 0){     //No es pot permetre, no hi ha res reservat.
		return NULL;  //uala que fem aqui?
	}
	int pags;
	if (n > 0){
		//fem el calcul de pagines
		pags = PAG_HEAP(brk+n) - PAG_HEAP(brk);
		ini_pag = inici_pag(brk);
		if (ini_pag)   //ens cal una pagineta més
			pags++;
		int frames[pags];
    int i;
    	for (i = 0; i < pags; i++){
      	frames[i] = alloc_frame();
        if (frames[i]<0){
        	for (int j = 0; j < i; j++)
          	free_frame(frames[j]);
        	return (void*) -ENOMEM;
      }       
    }
		for (i = 0; i < pags; i++){ 
    	set_ss_pag(pt, HEAP_FIRST_PAGE+PAG_HEAP(brk)+i+(!ini_pag), frames[i]);
    }
		brk = brk + n;
	} else if (n < 0) {   //toca borral
		unsigned int dest = (brk+n < L_HEAP_START)? L_HEAP_START : brk+n;
		ini_pag = inici_pag(dest);      //Estaré a inici de pàgina?
		pags = PAG_HEAP(brk)-PAG_HEAP(dest);
		if (ini_pag)
			++pags;
		int i;	
		for (i = 0; i < pags; i++){ 
    	del_ss_pag(pt, HEAP_FIRST_PAGE+PAG_HEAP(dest)+i+(!ini_pag));
    }
		brk = dest;
	}
	current()->brk= (void *) brk;
	return (void *) brk;
}























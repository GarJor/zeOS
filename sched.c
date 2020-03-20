/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));

#if 1  //Hem descomentat això
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif

extern struct list_head blocked;


struct list_head freequeue, readyqueue;
struct task_struct * idle_task;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	struct list_head *task_head = list_first(&freequeue);  //prenem l'element de la llista de fre
	list_del(task_head); //l'eliminem de la llista de free pq ja no esta free
	struct task_struct *idle_pcb = list_head_to_task_struct(task_head);
	idle_pcb->PID=0;	
	union task_union* utask = (union task_union *) idle_pcb; //agafem el task_union corresponent al PCB del idle
	utask->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle; //empilem la adressa de retorn
	utask->stack[KERNEL_STACK_SIZE - 2] = (unsigned long) 0;// empilem el fals ebp
	utask->task.kernel_esp = &(utask->stack[KERNEL_STACK_SIZE - 2]);	
	 
	allocate_DIR(idle_pcb); //assignem directori de pagines
	idle_task = idle_pcb;
	
}

void init_task1(void)
{
}

void init_freequeue(void)
{

	INIT_LIST_HEAD( &freequeue );
	int i;
	for(i=0; i<NR_TASKS; i++)
		list_add( &(task[i].task.list), &freequeue );
}
void init_readyqueue(void)
{
	
	INIT_LIST_HEAD( &readyqueue );

}

void init_sched()
{

}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}


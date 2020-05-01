#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	//runjp();

	
	char c [25][80]={{'c'}};
	put_screen(c);
	
  while(1) 
  {
		//char a;
//		if(get_key(&a) > 0 ) {
//		
//			write(1,&a,1);
//
//		} 
//	int i = 0;
//	char c [25][80]={{'c'}};
//	while(i++!=10000);
//	put_screen(c);
//	char a [25][80]={{'a'}};
//	put_screen(a);
//	while(i++!=10000);
//	char t [25][80]={{'t'}};
//	put_screen(t);	
			
  }
}

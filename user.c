#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	char *buff = "\nBenvingut al mode d'usuari. Aixo es un write jeje\n";	
	if( 0 > write(1,buff,strlen(buff)))
	{
		perror();	
  	} 

	buff = "gettime() retorna:\n";
  if( 0 > write(1,buff,strlen(buff)))
  {
                perror();
  }
	itoa(gettime(), buff);
	if( 0 > write(1,buff,strlen(buff)))
  {
                perror();
  }

	
  while(1) 
  {


  }
}

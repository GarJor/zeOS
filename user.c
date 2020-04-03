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
	int p = fork();
	if(p < 0) 
	{
		perror();
	}
	if(p == 0)
	{
		buff = "\nSoc el fill amb PID:\n";
	  if( 0 > write(1,buff,strlen(buff)))
	  {
	                perror();
	  }
		itoa(getpid(), buff);
		if( 0 > write(1,buff,strlen(buff)))
	  {
	                perror();
	  }

	}	else 
	{

		buff = "\nSoc el pare, amb PID:\n";
	  if( 0 > write(1,buff,strlen(buff)))
	  {
	                perror();
	  }
		itoa(getpid(), buff);
		if( 0 > write(1,buff,strlen(buff)))
	  {
	                perror();
	  }
		buff = "\nEl fill ha de tenir PID = ";
	  if( 0 > write(1,buff,strlen(buff)))
	  {
	                perror();
	  }
		itoa(p, buff);
		if( 0 > write(1,buff,strlen(buff)))
	  {
	                perror();
	  }
	}
  while(1) 
  {


  }
}

#define TOTAL 1f
#include <libc.h>




void init_scen(char scen[25][80]) {

	for (int i = 1; i < 24; i++) {
		scen[i][0] = '|';
		scen[i][79] = '|';
	} 
	for (int i = 1; i < 79; i++) {
		scen[0][i] = '-';
		scen[24][i] = '-';
	} 

		scen[0][0] = '+';
		scen[0][79] = '+';
		scen[24][0] = '+';
		scen[24][79] = '+';
}

void test_joc() {
	char buff[] = "TEST 2: provant la crida a sistema get_key() i put_screen()";
	write(1,buff,strlen(buff));
	char scen[25][80];
	init_scen(scen);
	int x = 12;
	int y = 40;
	char inp = 'U'; //0xb117ff 
	scen[x][y] = '@';
	while(inp != 'q') {

		get_key(&inp); //0xb11800
		if (inp == 'h') { //0xb117ff
			if(x > 1) {
				scen[x][y] = '\x00';
				scen[--x][y] = '@';
			}
		}

		if (inp == 'j') {
			if(y < 78 ) {
				scen[x][y] = '\x00';
				scen[x][++y] = '@';
			}
		}

		if (inp == 'k') {
			if(y > 1) {
				scen[x][y] = '\x00';
				scen[x][--y] = '@';
			}
		}
		if (inp == 'l') {
			if(x < 23) {
				scen[x][y] = '\x00';
				scen[++x][y] = '@';
			}
		}
	  put_screen(scen);
		
	}
}

void test_get_key(){
	char buff[] = "\nTEST 1: provant la crida a sistema get_key()";
	write(1,buff,strlen(buff));
	char buff2[] = "\nEscriu tecles si us plau\n";
	write(1,buff2,strlen(buff2));
	unsigned long t = gettime();
	while(gettime() <= t+1100);
	char buff3[] = "s'han de mostrar les últimes 8 lletres que has premut\n";
	write(1,buff3,strlen(buff3));
	int i = 8;
	while(i-->0){
		char a;
		get_key(&a);
		a='A';
		write(1,&a,1);
		write(1," ",1);
    
	}	
		write(1,"\n",1);
}

void jp_all() {
	test_get_key();
//	test_joc();
}

void jp_rank(){

	write(1,"testR\n",6);
}

#define TOTAL 1f
#include <libc.h>


void flush_screen(char scen[25][80]) {
	for(int i = 0; i < 25; ++i) {
		for(int j = 0; j < 80; ++j) scen[i][j] = '\x00';
	}

}


void init_scen(char scen[25][80]) {
	char txt[25] = "Mou-te amb les tecles vim";
	for(int i = 0; i < 25; ++i) scen[3][3+i] = txt[i];
	char	txt2[19] = "Prem 'q' per sortir";
	for(int i = 0; i < 19; ++i) scen[4][3+i] = txt2[i];

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

void omple(char a[], char *b) {

	for(int i = 0; i < strlen(a); ++i) *b++ = a[i];

}
void procrea(int fills) {

	for(int i = 0; i < fills; ++i) {
		int p = fork();
		if(p == 0) {
			while(1);
		}
	
	}

}


void test_fps() {
	char scen[25][80];
	flush_screen(scen);
	omple("TEST 3: Calcul de fps",&scen[3][3]);
	procrea(1);
	set_fps(80);
	int max = 10000;
	int i = 0;
	int j = 0;
	int ini = gettime();
	while(i < max){
		j += put_screen((char *)scen);
		++i;
	}
	set_fps(-1);
	int currtime = gettime()-ini;
	int res = (currtime == 0)? j : (j*18)/currtime; 
	itoa(res,&scen[5][9]);
	itoa(currtime,&scen[6][9]);
	itoa(j,&scen[7][9]);

	omple("fps", &scen[5][17]);
	put_screen((char *)scen); // hauriem de guardar lultim putscreen per printarlo al final perk sino perdem el estat del final
//	write(1,&scen[5][9],11);
}


void test_joc() {
	char buff[] = "TEST 2: provant la crida a sistema get_key() i put_screen()";
	write(1,buff,strlen(buff));
	char scen[25][80];
	flush_screen(scen);
	init_scen(scen);
	int x = 12;
	int y = 40;
	char inp = 'U'; //0xb117ff 
	scen[x][y] = '@';
	while(inp != 'q') {

		get_key(&inp); //0xb11800
		if (inp == 'k') { //0xb117ff
			if(x > 1) {
				scen[x][y] = '\x00';
				scen[--x][y] = '@';
			}
		}

		if (inp == 'l') {
			if(y < 78 ) {
				scen[x][y] = '\x00';
				scen[x][++y] = '@';
			}
		}

		if (inp == 'h') {
			if(y > 1) {
				scen[x][y] = '\x00';
				scen[x][--y] = '@';
			}
		}
		if (inp == 'j') {
			if(x < 23) {
				scen[x][y] = '\x00';
				scen[++x][y] = '@';
			}
		}
	  put_screen((char *)scen);
		
	}
}

void test_get_key(){
	char buff[] = "\nTEST 1: provant la crida a sistema get_key()";
	write(1,buff,strlen(buff));
	char buff2[] = "\nEscriu tecles si us plau en els propers 60 segons del zeOS. S'han de mostrar les ultimes 8 premudes.\n";
	write(1,buff2,strlen(buff2));
	unsigned long t = gettime();
	while(gettime() <= t+1080);
	char buff3[] = "Les ultimes 8 lletres que has premut:\n";
	write(1,buff3,strlen(buff3));
	int i = 8;
	while(i-->0){
		char a;
		get_key(&a);
		write(1,&a,1);
		write(1," ",1);
    
	}	
		write(1,"\n",1);
}

void jp_all() {
	test_get_key(); // 1
	test_joc(); // 2
	test_fps(); // 3
}

void jp_rank(int ini, int fin){

	for(int i = ini ; i <= fin; i++) {

		switch(i) {
			case 1:
				test_get_key();
				break;
			case 2:
				test_joc();
				break;
			case 3:
				test_fps();
				break;
		}


	} 
}

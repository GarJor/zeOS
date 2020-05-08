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


int test_fps(int fps) {
	char scen[25][80];
	flush_screen(scen);
	omple("TEST 3: Comprovacio de set_fps() amb diversos procesos",&scen[3][3]);
	set_fps(fps);
	int iter = 0;
	int res = 0;
	while(iter < 4){
		//procrea(iter);
		int max = 1000;
		int i = 0;
		int j = 0;
		int ini = gettime();
		while(i < max){
			j += put_screen((char *)scen);
			++i;
		}
		int spent = gettime()-ini;
		int segs = spent/18;
		segs = (segs*18 < spent)? segs+1 : segs; // Arrodonim cap amunt
		res = (segs == 0)? j : j/segs; 
		res = (res*segs < j)? res+1 : res; // Arrodonim cap amunt 
		if ( res != fps) break; 
		iter += 2;
	}
	set_fps(-1);
	if(iter < 6) {

		char buff3[] = "TEST 3: FAIL (/)\n";
		write(1,buff3,strlen(buff3));
		return 0;
	}
	else {
		char buff3[] = "TEST 3: OK\n";
		write(1,buff3,strlen(buff3));
		return 1;

	}
}


void test_joc() {
	char buff[] = "TEST 0: provant la crida a sistema get_key() i put_screen()\n";
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
	char buff1[] = "TEST 0: ? (Creus que va be?) \n";
	write(1,buff1,strlen(buff1));
	flush_screen(scen);
	put_screen((char *)scen);

}

int test_get_key(){
	char buff[] = "\nTEST 2: provant la crida a sistema get_key()";
	write(1,buff,strlen(buff));
	char buff2[] = "\nEscriu en ordre els seguents caracters: q w e r t y u i o p\n";
	write(1,buff2,strlen(buff2));
	unsigned long t = gettime();
	while(gettime() <= t+1080);
	char check[8] = "opertyui";
	int i = 0;

	while(i<8){
		char a;
		get_key(&a);
   	if ( a != check[i]) break; 
		++i;
	}	
	if ( i < 8 ) { // No passa el test
		char buff3[] = "TEST 2: FAIL (segur que has premut els caracters indicats?)\n";
		write(1,buff3,strlen(buff3));
		return 0;

	} else // passa el test
	{

		char buff3[] = "TEST 2: OK\n";
		write(1,buff3,strlen(buff3));
		return 1;
	}
}

void jp_all() {
	int total = 0;
	test_joc(); // 1
	total += test_get_key(); // 2
	total += test_fps(70); // 3
}

void jp_rank(int ini, int fin){
	int unknown = 0;
	int total = fin - ini +1;
	int failed = total; 
	for(int i = ini ; i <= fin; i++) {

		switch(i) {
			case 0:
				test_joc();
				++unknown;
				--failed;
				break;
			case 1:
				failed -= test_get_key();
				break;
			case 2:
				failed -= test_fps(70);
				break;
		}


	} 
	char scen[25][80];
	flush_screen(scen);
	put_screen((char *)scen);
	char res[] = "\n\nFAILED TESTS:   \nUNKNOWN RESULTS:   \nTOTAL:   \n";
	itoa(failed, &res[16]);
	itoa(unknown, &res[36]);
	itoa(total, &res[46]);
	write(1, res,48);

}

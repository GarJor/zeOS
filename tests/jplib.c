#include <utils.h>


#define TOTAL 1





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

void test_joc() {

	char scen[25][80];
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
	  put_screen(scen);
		
	}
}



void jp_all() {
	test_joc();
}

void jp_rank(){

	write(1,"testR\n",6);
}

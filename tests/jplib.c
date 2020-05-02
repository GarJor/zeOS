#define TOTAL 1





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

	char scen[25][80];
	init_scen(scen);
	int x = 12;
	int y = 40;
	char inp = 'U';
	
	scen[x][y] = '@';
	while(inp != 'q') {
	scen[0][0] = inp;
		if (inp == 'h') {
			if(x > 1) {
				scen[x][y] = ' ';
				scen[--x][y] = '@';
			}
		}

		if (inp == 'j') {
			if(y < 78) {
				scen[x][y] = ' ';
				scen[x][++y] = '@';
			}
		}

		if (inp == 'k') {
			if(y > 1) {
				scen[x][y] = ' ';
				scen[x][--y] = '@';
			}
		}
		if (inp == 'l') {
			if(x < 23) {
				scen[x][y] = ' ';
				scen[++x][y] = '@';
			}
		}
		put_screen(scen);
		get_key(&inp);
	}
}



void jp_all() {
	test_joc();
}

void jp_rank(){

	write(1,"testR\n",6);
}

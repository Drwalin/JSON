
#include "../include/JSON.hpp"

#include <cstdlib>
#include <ctime>
#include <fstream>

std::string RS() {
	std::string s;
	int l=(rand()%50) + 5;
	for(int i = 0; i<l; ++i)
		s += (char)(rand()%80) + ' ';
	return s;
}

void Random(JSON& j, int d) {
	int l;
	switch((rand()%23)-d) {
	case 0:
		l = (rand() % 10) + 10;
		for(int i=0; i<l; ++i) {
			Random(j[RS()], d-1);
		}
		break;
	case 1:
		l = (rand() % 10) + 10;
		for(int i=0; i<l; ++i) {
			Random(j[i], d-1);
		}
		break;
	default:
		switch(rand()%6) {
		case 0:
			j = (bool)(rand()&1);
			break;
		case 1:
			j = rand();
			break;
		case 2:
			j = (double)rand();
			break;
		default:
			j = RS();
		}
	}
}

int main(int argc, char**argv) {
	srand(time(NULL));
	if(argc>1)
	{
		JSON j;
		for(int i=0; i<10000; ++i)
			Random(j[i], 9);
		std::ofstream f("test2_benchmark.json");
		j.WriteBeautyfull(f, "    ", "\n");
	}

	{
		JSON j;
		std::ifstream f("test2_benchmark.json");
		int beg = clock();
		f >> j;
		int end = clock()-beg;
		float t = (float)(end)/(float)CLOCKS_PER_SEC;
		printf("\n loaded in: %f s\n", t);
	}

	return 0;
}


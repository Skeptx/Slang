#include <cstdio>
#include <iostream>
#include <unordered_map>

#include "libslang.h"

using namespace std;

void readIntoMap(unordered_map<unsigned long long int, bool> *map) {
	FILE *file = fopen("words.dat", "rb");
	if (!file) {
		perror("Error: fopen failed");
		exit(EXIT_FAILURE);
	}
	unsigned long long int key = 0LL;
	while (fread(&key, 5, 1, file)) {
		(*map)[key] = true;
	}
	if (fclose(file)) {
		perror("Error: fclose failed");
	}
}

void messageHandler(char *message) {
}

int main() {
	unordered_map<unsigned long long int, bool> map;
	//readIntoMap(&map);
	SlangLib slang('S', messageHandler);
	slang.init(46257);
	return 0;
}
#include "Map.h"

Map::Map() {
	resetMap();
}

void Map::printMap() {
	for (int i = 0; i < charMap.size(); i++) {
		for (int j = 0; j < charMap[i].size(); j++) {
			std::cout << charMap[i][j];
		}
		std::cout << std::endl;
	}
}

void Map::resetMap() {
	charMap.clear();
	for (int i = 0; i < 10; i++) {
		std::vector<char> tmp;
		for (int j = 0; j < 10; j++) {
			tmp.push_back('E');
		}
		charMap.push_back(tmp);
	}

	srand((unsigned)time(NULL));
	// segment first block, block size must be 2 or 4
	int size = ((rand() % 2) == 0) ? 2 : 4;
	for (int i = 0; i < 10; i++) {
		charMap[size][i] = (charMap[size][i] == '|') ? '+' : '-';
		charMap[i][size] = (charMap[i][size] == '-') ? '+' : '|';
		if (size * 2 + 1 < 10) {
			charMap[size*2 + 1][i] = (charMap[size * 2 + 1][i] == '|') ? '+' : '-';
			charMap[i][size*2 + 1] = (charMap[i][size * 2 + 1] == '-') ? '+' : '|';
		}
	}

	srand((unsigned)time(NULL) + 1);
	int tIndex = rand() % 9;
	while (!(charMap[tIndex][tIndex] == 'E' && charMap[tIndex + 1][tIndex] == 'E' && charMap[tIndex + 1][tIndex + 1] == 'E'
		&& charMap[tIndex][tIndex + 1] == 'E')) {
		srand((unsigned)time(NULL)+2);
		tIndex = rand() % 9;
		//std::cout << tIndex << std::endl;
	}

	charMap[tIndex][tIndex] = 'T';
	charMap[tIndex+1][tIndex] = 'T';
	charMap[tIndex][tIndex+1] = 'T';
	charMap[tIndex+1][tIndex+1] = 'T';

	srand((unsigned)time(NULL)*2);
    tIndex = rand() % 9;
	while (!(charMap[tIndex][tIndex] == 'E' && charMap[tIndex + 1][tIndex] == 'E' && charMap[tIndex + 1][tIndex + 1] == 'E'
		&& charMap[tIndex][tIndex + 1] == 'E')) {
		srand((unsigned)time(NULL)*3 + 520);
		tIndex = rand() % 9;
		//std::cout << tIndex << std::endl;
	}

	charMap[tIndex][tIndex] = 'A';
	charMap[tIndex + 1][tIndex] = 'A';
	charMap[tIndex][tIndex + 1] = 'A';
	charMap[tIndex + 1][tIndex + 1] = 'A';

	for (int i = 0; i < charMap.size(); i++) {
		for (int j = 0; j < charMap[i].size(); j++) {
			if (charMap[i][j] == 'E') {
				charMap[i][j] = 'B';
			}
		}
	}
}
#ifndef PATTERN_DATABASE_H
#define PATTERN_DATABASE_H

#include "../Model/cubeIndexModel.h"

#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <iostream>

using namespace std;

class PatternDatabase {
public:
	map<int64_t, int> dataBase;
	thread readThread;

	PatternDatabase() {
	}

	void readFromFile(string fileName) {
		int64_t hash;
		int moves;
		std::ifstream input(fileName);
		while (input >> hash >> moves)
			dataBase[hash] = moves;
		input.close();
		std::cout << "Finished reading from: " << fileName << "\n";
	}

	virtual int64_t getDatabaseIndex(CubeIndexModel cube) {
		cout << "Failed to initialize getDatabaseIndex\n";
		return 0;
	}

	virtual int getNumMoves(CubeIndexModel cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return dataBase[ID];
	}
	

};

#endif // !PATTERN_DATABASE_H

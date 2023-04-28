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
	vector<int> database;
	map<int64_t, int> hashDatabase;
	thread readThread;

	void readHashData(string fileName) {
		ifstream input(fileName);
		int64_t hash;
		int numMoves;
		while (input >> hash >> numMoves)
			hashDatabase[hash] = numMoves;
		input.close();
		std::cout << "Finished reading from: " << fileName << "\n";
	}

	void readLehmerData(string fileName) {
		ifstream input(fileName);
		string data;
		input >> data;
		for (int i = 0; i < data.length(); i++)
			database[i] = (int)(data[i] - 'a');
		input.close();
		std::cout << "Finished reading from: " << fileName << "\n";
	}

	virtual int64_t getDatabaseIndex(const CubeIndexModel& cube) {
		cout << "Failed to initialize getDatabaseIndex\n";
		return 0;
	}

	virtual int getNumMoves(const CubeIndexModel& cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return database[ID];
	}
};

#endif // !PATTERN_DATABASE_H

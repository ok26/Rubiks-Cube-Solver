#ifndef THISTLEWAITE_G1_H
#define THISTLEWAITE_G1_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG1 : public PatternDatabase {
public:

	PatternDatabaseG1() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readHashData, this, "resources/data/ThistlewaiteG1");
	};

	int64_t getDatabaseIndex(const CubeIndexModel& cube) {
		int64_t ID = 1;

		for (int i = 0; i < 12; i++) {
			ID = ID << 1;
			ID += cube.edges[i].orientation;
		}

		return ID;
	}

	int getNumMoves(const CubeIndexModel& cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return hashDatabase[ID];
	}
};

#endif
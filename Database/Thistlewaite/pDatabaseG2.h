#ifndef THISTLEWAITE_G2_H
#define THISTLEWAITE_G2_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG2 : public PatternDatabase {
public:

	PatternDatabaseG2() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readHashData, this, "resources/data/ThistlewaiteG2");
	}

	int64_t getDatabaseIndex(const CubeIndexModel& cube) {
		int64_t ID = 1;

		for (int i = 0; i < 7; i++) {
			ID = ID << 2;
			ID += cube.corners[i].orientation;
		}

		for (int i = 0; i < 12; i++) {
			ID = ID << 1;
			int index = cube.edges[i].index;
			if (index > 3 && index < 8)
				ID++;
		}

		return ID;
	}

	int getNumMoves(const CubeIndexModel& cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return hashDatabase[ID];
	}
};

#endif
#ifndef KORF_G2_CORNERS_H
#define KORF_G2_CORNERS_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG2Corners : public PatternDatabase {
public:

	PatternDatabaseG2Corners() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readFromFile, this, "resources/data/KorfG2Corners");
	}

	int64_t getDatabaseIndex(CubeIndexModel cube) {
		int64_t ID = 1;

		for (int i = 0; i < 8; i++) {
			ID = ID << 3;
			ID += cube.corners[i].index;
		}

		for (int i = 4; i < 8; i++) {
			ID = ID << 4;
			ID += cube.edges[i].index;
		}

		return ID;
	}

	int getNumMoves(CubeIndexModel cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return dataBase[ID];
	}
};

#endif
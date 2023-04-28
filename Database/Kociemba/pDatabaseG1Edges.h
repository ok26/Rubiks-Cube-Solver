#ifndef KORF_G1_EDGES_H
#define KORF_G1_EDGES_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG1Edges : public PatternDatabase {
public:

	PatternDatabaseG1Edges() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readFromFile, this, "resources/data/KorfG1Edges");
	}

	int64_t getDatabaseIndex(CubeIndexModel cube) {
		int64_t ID = 1;

		for (int i = 0; i < 12; i++) {
			ID = ID << 1;
			ID += cube.edges[i].orientation;
		}

		for (int i = 0; i < 12; i++) {
			ID = ID << 1;
			int index = cube.edges[i].index;
			if (index > 3 && index < 8)
				ID++;
		}

		return ID;
	}

	int getNumMoves(CubeIndexModel cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return dataBase[ID];
	}
};

#endif
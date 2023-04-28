#ifndef KORF_G2_EDGES_H
#define KORF_G2_EDGES_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG2Edges : public PatternDatabase {
public:

	PatternDatabaseG2Edges() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readFromFile, this, "resources/data/KorfG2Edges");
	}

	int64_t getDatabaseIndex(CubeIndexModel cube) {
		int64_t ID = 1;

		for (int i = 0; i < 12; i++) {
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
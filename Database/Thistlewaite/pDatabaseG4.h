#ifndef THISTLEWAITE_G4_H
#define THISTLEWAITE_G4_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG4 : public PatternDatabase {
public:

	PatternDatabaseG4() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readFromFile, this, "resources/data/ThistlewaiteG4");
	}

	int64_t getDatabaseIndex(CubeIndexModel cube) {
		int64_t ID = 1;

		for (int i = 0; i < 12; i++) {
			if (cube.edges[i].index < 4)
				ID = ID << 2;
			else if (cube.edges[i].index < 8)
				ID = ID << 3;
			else
				ID = ID << 4;
			ID += cube.edges[i].index;
		}
		for (int i = 0; i < 8; i++) {
			ID = ID << 3;
			ID += cube.corners[i].index;
		}

		return ID;
	}

	int getNumMoves(CubeIndexModel cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return dataBase[ID];
	}
};

#endif
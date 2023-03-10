#ifndef THISTLEWAITE_G3_H
#define THISTLEWAITE_G3_H

#include "../patternDatabase.h"

using namespace std;

class PatternDatabaseG3 : public PatternDatabase {
public:

	PatternDatabaseG3() : PatternDatabase() {
		readThread = thread(&PatternDatabase::readFromFile, this, "resources/data/ThistlewaiteG3");
	}

	int64_t getDatabaseIndex(CubeIndexModel cube) {
		int64_t ID = 1;

		array<Cubie, 8> cornersCopy = cube.corners;
		if (cube.corners[0].index == 2 && cube.corners[2].index == 0) swap(cornersCopy[0], cornersCopy[2]);
		if (cube.corners[1].index == 3 && cube.corners[3].index == 1) swap(cornersCopy[1], cornersCopy[3]);
		if (cube.corners[4].index == 6 && cube.corners[6].index == 4) swap(cornersCopy[4], cornersCopy[6]);
		if (cube.corners[5].index == 7 && cube.corners[7].index == 5) swap(cornersCopy[5], cornersCopy[7]);

		for (int i = 0; i < 8; i++) {
			ID = ID << 3;
			ID += cornersCopy[i].index;
		}
		
		for (int i = 0; i < 12; i++) {
			ID = ID << 1;
			int index = cube.edges[i].index;
			if (index == 0 || index == 2 || index == 8 || index == 10)
				ID++;
		}
		int parity = 0;
		for (int i = 0; i < 8; i++)
			for (int j = i + 1; j < 8; j++)
				parity ^= (cube.corners[i].index < cube.corners[j].index);
		ID = ID << 1;
		ID += parity;

		return ID;
	}

	int getNumMoves(CubeIndexModel cube) {
		int64_t ID = this->getDatabaseIndex(cube);
		return dataBase[ID];
	}
};

#endif
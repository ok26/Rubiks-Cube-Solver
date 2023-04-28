#ifndef CUBE_SCRAMBLE_H
#define CUBE_SCRAMBLE_H

#include "../Model/cubeIndexModel.h"

#include <random>
#include <queue>

using namespace std;

class CubeScrambler {
public:
	CubeScrambler(unsigned int seed) {
		srand(seed);
	}

	void scrambleCube100(queue<Move> *moveQueue) {
		for (int i = 0; i < 100; i++) {
			Move move;
			move.target = rand() % 6;
			while (!moveQueue->empty() && move.target == moveQueue->back().target)
				move.target = rand() % 6;
			move.duration = 0.01f;
			move.axis = getAxis(move.target);
			move.inverse = rand() % 2;
			moveQueue->push(move);
		}
	}

	void scrambleCubeWithRandomMoveNr(queue<Move>* moveQueue, int upperBound, int lowerBound, float animationDuration) {
		int moveNr = (rand() % (upperBound - lowerBound + 1)) + lowerBound;
		for (int i = 0; i < moveNr; i++) {
			Move move;
			move.target = rand() % 6;
			while (!moveQueue->empty() && move.target == moveQueue->back().target)
				move.target = rand() % 6;
			move.duration = animationDuration;
			move.axis = getAxis(move.target);
			move.inverse = rand() % 2;
			moveQueue->push(move);
		}
	}

	int getAxis(int targetSide) {
		if (targetSide == 0 || targetSide == 5)
			return 1;
		if (targetSide == 1 || targetSide == 4)
			return 2;
		if (targetSide == 2 || targetSide == 3)
			return 0;
		return 0;
	}
};

#endif // !CUBE_SCRAMBLE_H
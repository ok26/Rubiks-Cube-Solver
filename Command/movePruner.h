#ifndef MOVE_PRUNER_H
#define MOVE_PRUNER_H

#include "../Model/cubeIndexModel.h"

class MovePruner {
public:
	MovePruner() {}

	bool pruneMove(int move, int lastMove) {
		if ((move == F || move == Fp || move == F2) &&
			(lastMove == F || lastMove == Fp || lastMove == F2))
			return true;
		if ((move == B || move == Bp || move == B2) &&
			(lastMove == B || lastMove == Bp || lastMove == B2))
			return true;
		if ((move == U || move == Up || move == U2) &&
			(lastMove == U || lastMove == Up || lastMove == U2))
			return true;
		if ((move == D || move == Dp || move == D2) &&
			(lastMove == D || lastMove == Dp || lastMove == D2))
			return true;
		if ((move == L || move == Lp || move == L2) &&
			(lastMove == L || lastMove == Lp || lastMove == L2))
			return true;
		if ((move == R || move == Rp || move == R2) &&
			(lastMove == R || lastMove == Rp || lastMove == R2))
			return true;
		if ((move == R || move == Rp || move == R2) &&
			(lastMove == L || lastMove == Lp || lastMove == L2))
			return true;
		if ((move == F || move == Fp || move == F2) &&
			(lastMove == B || lastMove == Bp || lastMove == B2))
			return true;
		if ((move == U || move == Up || move == U2) &&
			(lastMove == D || lastMove == Dp || lastMove == D2))
			return true;
		return false;
	}
};

#endif // !MOVE_PRUNER_H

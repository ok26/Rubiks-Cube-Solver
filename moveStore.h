#ifndef MOVE_STORE_H
#define MOVE_STORE_H

#include "../Model/cubeIndexModel.h"

#include <array>
#include <string>
#include <vector>
#include <array>

using namespace std;

struct Move {
	int target;
	float duration;
	int axis;
	bool inverse;
};

class MoveStore {
public:
	array<string, 18> cubeMoveNames = { "R", "R'", "R2", "L", "L'", "L2",
										"U", "U'", "U2", "D", "D'", "D2",
										"F", "F'", "F2", "B", "B'", "B2" };
	vector<int> targetedFace = { 2, 3, 0, 5, 1, 4 };

	void printMoveSequence(vector<int> sequence) {
		for (int move : sequence)
			cout << cubeMoveNames[move] << " ";
		cout << "\n";
	}

	//Only supports symCoord 0 and 1 for triple search
	std::vector<int> translateSymmetryMoves(vector<int> moves, int symmetryCoord) {
		std::array<int, 18> moveTranslations;
		if (symmetryCoord == 0) moveTranslations = { U, Up, U2, D, Dp, D2, L, Lp, L2, R, Rp, R2, F, Fp, F2, B, Bp, B2 };
		else moveTranslations = { R, Rp, R2, L, Lp, L2, F, Fp, F2, B, Bp, B2, D, Dp, D2, U, Up, U2 };

		std::vector<int> translatedMoves;
		for (int i = 0; i < moves.size(); i++)
			translatedMoves.push_back(moveTranslations[moves[i]]);
		return translatedMoves;
	}

	vector<Move> indexToAnimationMove(vector<int> moveSequence, float duration) {
		vector<Move> converted;

		for (int move : moveSequence) {
			Move convertedMove;
			convertedMove.target = targetedFace[move / 3];
			convertedMove.axis = move / 6;
			convertedMove.duration = duration;

			if (move % 3 == 1)
				convertedMove.inverse = true;
			else
				convertedMove.inverse = false;

			//temporary
			if ((move / 3) % 2 == 0)
				convertedMove.inverse = !convertedMove.inverse;

			//still temporary
			if (move % 3 == 2)
				converted.push_back(convertedMove);

			//not temporary
			converted.push_back(convertedMove);
		}
		return converted;
	}

	int animationMoveToIndex(Move move) {
		int diff = move.inverse;
		if (move.target < 3)
			diff = 1 - diff;
		return 3 * (find(targetedFace.begin(), targetedFace.end(), move.target) - targetedFace.begin()) + diff;
	}
};

#endif // !MOVE_STORE_H

#ifndef SIMPLIFY_SOLUTION_H
#define SIMPLIFY_SOLUTION_H

#include "../Model/cubeIndexModel.h"

#include <vector>

using namespace std;

class SolutionSimplifier {
public:

	void updateSolution(vector<int>& solution, int index, int move, int lastMove) {
		solution.erase(solution.begin() + index - 1, solution.begin() + index + 1);

		if (move == lastMove && move % 3 < 2)
			solution.insert(solution.begin() + index - 1, move - (move % 3) + 2);

		else if ((move % 3 == 2 || lastMove % 3 ==  2) && move != lastMove) {
			if (((move % 3) + (lastMove % 3)) % 3 == 0)
				solution.insert(solution.begin() + index - 1, move - (move % 3));
			else
				solution.insert(solution.begin() + index - 1, move - (move % 3) + 1);
		}
	}
	
	void simplifySolution(vector<int>& solution) {
		for (int i = 1; i < solution.size(); i++) {

			int move = solution.at(i), lastMove = solution.at(i - 1);

			if ((move == F || move == Fp || move == F2) &&
				(lastMove == F || lastMove == Fp || lastMove == F2))

				updateSolution(solution, i, move, lastMove);

			else if ((move == B || move == Bp || move == B2) &&
				(lastMove == B || lastMove == Bp || lastMove == B2))

				updateSolution(solution, i, move, lastMove);

			else if ((move == U || move == Up || move == U2) &&
				(lastMove == U || lastMove == Up || lastMove == U2))

				updateSolution(solution, i, move, lastMove);
			
			else if ((move == D || move == Dp || move == D2) &&
				(lastMove == D || lastMove == Dp || lastMove == D2))

				updateSolution(solution, i, move, lastMove);
			
			else if ((move == R || move == Rp || move == R2) &&
				(lastMove == R || lastMove == Rp || lastMove == R2))

				updateSolution(solution, i, move, lastMove);
		
			else if ((move == L || move == Lp || move == L2) &&
				(lastMove == L || lastMove == Lp || lastMove == L2))

				updateSolution(solution, i, move, lastMove);
		}
	}

	int sameMoveReductionDiff(int move1, int move2) {
		if (move1 == move2 && move1 % 3 < 2)
			return 1;

		else if ((move1 % 3 == 2 || move2 % 3 == 2) && move1 != move2) {
			return 1;
		}
		return 2;
	}

	int reductionDifference(int move1, int move2) {
		if ((move1 == F || move1 == Fp || move1 == F2) &&
			(move2 == F || move2 == Fp || move2 == F2))

			return sameMoveReductionDiff(move1, move2);

		else if ((move1 == B || move1 == Bp || move1 == B2) &&
			(move2 == B || move2 == Bp || move2 == B2))

			return sameMoveReductionDiff(move1, move2);

		else if ((move1 == U || move1 == Up || move1 == U2) &&
			(move2 == U || move2 == Up || move2 == U2))

			return sameMoveReductionDiff(move1, move2);

		else if ((move1 == D || move1 == Dp || move1 == D2) &&
			(move2 == D || move2 == Dp || move2 == D2))

			return sameMoveReductionDiff(move1, move2);

		else if ((move1 == R || move1 == Rp || move1 == R2) &&
			(move2 == R || move2 == Rp || move2 == R2))

			return sameMoveReductionDiff(move1, move2);

		else if ((move1 == L || move1 == Lp || move1 == L2) &&
			(move2 == L || move2 == Lp || move2 == L2))

			return sameMoveReductionDiff(move1, move2);
		
		return 0;
	}
};

#endif

#ifndef CUBE_SOLVER_H
#define CUBE_SOLVER_H

#include "../Model/cubeIndexModel.h"
#include "../Database/patternDatabase.h"
#include "movePruner.h"

#include <queue>
#include <map>
#include <vector>
#include <stack>

using namespace std;

struct Node {
	CubeIndexModel cube;
	int move;
	int depth;
};

struct PrioritizedMove {
	CubeIndexModel cube;
	int move;
	int estMoves;
	bool operator>(const PrioritizedMove rhs) const {
		return this->estMoves > rhs.estMoves;
	}
};


class CubeSolver {
public:

	vector<int> solveBFS(PatternDatabase* pDatabase, CubeIndexModel cube, int availableMoves) {

		vector<int> solution;
		MovePruner movePruner;
		
		int64_t currentID = pDatabase->getDatabaseIndex(cube);
		CubeIndexModel solvedState;
		int64_t solvedID = pDatabase->getDatabaseIndex(solvedState);

		if (currentID == solvedID)
			return solution;

		queue<CubeIndexModel> q;
		q.push(cube);
		map<int64_t, int64_t> predecessor;
		map<int64_t, int> direction, lastMove;
		direction[currentID] = 1;
		direction[solvedID] = 2;

		while (!q.empty()) {
			CubeIndexModel oldState = q.front();
			q.pop();
			int64_t oldID = pDatabase->getDatabaseIndex(oldState);
			int& oldDir = direction[oldID];

			for (int move = 0; move < 18; move++) {
				if (availableMoves & (1 << move) && !movePruner.pruneMove(move, lastMove[oldID])) {

					CubeIndexModel newState = oldState;
					newState.doMove(move);
					int64_t newID = pDatabase->getDatabaseIndex(newState);
					int& newDir = direction[newID];

					if (newID == solvedID) {
						vector<int> algorithm(1, move);
						while (oldID != currentID) {
							algorithm.insert(algorithm.begin(), lastMove[oldID]);
							oldID = predecessor[oldID];
						}

						for (int i = 0; i < (int)algorithm.size(); i++) {
							cube.doMove(algorithm[i]);
							solution.push_back(algorithm[i]);
						}
						return solution;
					}

					if (!newDir) {
						q.push(newState);
						newDir = oldDir;
						lastMove[newID] = move;
						predecessor[newID] = oldID;
					}
				}
			}
		}
		return solution;
	}

	vector<int> solveIDA(PatternDatabase* pDatabase, CubeIndexModel cube, int availableMoves, int foundSolutionDepth = 1e9) {
		typedef priority_queue<PrioritizedMove, vector<PrioritizedMove>, greater<PrioritizedMove>> moveQueue_t;
		MovePruner movePruner;

		stack<Node> nodeStack;
		Node curNode;
		array<int, 50> moves = { (int)0xFF };
		bool solved = false;
		int bound = 0;
		int nextBound = pDatabase->getNumMoves(cube);
		
		if (nextBound == 0)
			solved = true;

		while (!solved) {
			if (nodeStack.empty()) {

				nodeStack.push({ cube, (int)0xFF, 0 });

				if (nextBound == 0 || nextBound == 0xFF)
					cout << "HOW?";

				bound = nextBound;
				nextBound = 0xFF;
			}

			if (bound >= foundSolutionDepth) {
				return vector<int>(bound, -1);
			}

			curNode = nodeStack.top();
			nodeStack.pop();

			moves.at(curNode.depth) = (int)0xFF;

			if (curNode.depth != 0)
				moves[curNode.depth - 1] = curNode.move;

			if (curNode.depth == bound) {
				if (pDatabase->getNumMoves(curNode.cube) == 0)
					solved = true;
			}
			else {
				moveQueue_t successors;

				for (int move = 0; move < 18; move++) {
					if (availableMoves & (1 << move)) {

						if (curNode.depth == 0 || !movePruner.pruneMove(move, curNode.move)) {
							CubeIndexModel cubeCopy(curNode.cube);
							cubeCopy.doMove(move);

							int estSuccMoves = curNode.depth + 1 + pDatabase->getNumMoves(cubeCopy);

							if (estSuccMoves <= bound) {
								successors.push({ cubeCopy, move, estSuccMoves });
							}
							else if (estSuccMoves < nextBound) {
								nextBound = estSuccMoves;
							}
						}
					}
				}

				while (!successors.empty()) {
					nodeStack.push({
						successors.top().cube,
						successors.top().move,
						(int)(curNode.depth + 1)
						});
					successors.pop();
				}
			}
		}
		vector<int> solution;
		for (int i = 0; i < moves.size() && (int)moves.at(i) != 0xFF; i++)
			solution.push_back(moves.at(i));
			
		return solution;
	}
};

#endif // !CUBE_SOLVER_H
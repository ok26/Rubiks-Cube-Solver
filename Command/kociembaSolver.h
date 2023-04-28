#ifndef IMPRV_KOCIMEBA_SOLVER_H
#define IMPRV_KOCIMEBA_SOLVER_H

#include "../Model/cubeIndexModel.h"

#include "../Database/Kociemba/g1DatabaseController.h"
#include "../Database/Kociemba/moveController.h"
#include "../Database/Kociemba/g2DatabaseController.h"

#include "../Database/moveStore.h"
#include "../Database/patternIndexer.h"

#include "cubeSolver.h"
#include "simplifySolution.h"

#include <iostream>
#include <queue>
#include <vector>
#include <stack>

#include <chrono>


struct CubeStateG1 {
	int flipUDSlice;
	int cornerOri;
	int cornerPerm;
	int UDSliceSort;
	int USliceSort;
	int DSliceSort;
};

struct CubeStateG2 {
	int cornerPerm;
	int edgeRPerm;
	int edgeMPerm;
};

struct NodeG1 {
	CubeStateG1 cube;
	int move;
	int depth;
};

struct NodeG2 {
	CubeStateG2 cube;
	int move;
	int depth;
};

struct PrioritizedMoveG1 {
	CubeStateG1 cube;
	int move;
	int estMoves;
	bool operator>(const PrioritizedMoveG1 rhs) const {
		return this->estMoves > rhs.estMoves;
	}
};

struct PrioritizedMoveG2 {
	CubeStateG2 cube;
	int move;
	int estMoves;
	bool operator>(const PrioritizedMoveG2 rhs) const {
		return this->estMoves > rhs.estMoves;
	}
};


class KociembaSolver {

	CubeIndexer cubeIndexer;
	G1DatabaseController databaseG1Controller;
	G2DatabaseController databaseG2Controller;
	MoveController moveController;

	PatternDatabase* kociembaDatabaseG2;

	int availableMovesG2 = 151524;

public:
	bool tablesInitialized = false;

	KociembaSolver(bool initializeTablesOnCreate = true) {

		if (initializeTablesOnCreate) 
			initializeTables();
	}

	void initializeTables() {
		databaseG1Controller.initializeTables();
		databaseG2Controller.initializeTables();
		moveController.initializeTables();
		tablesInitialized = true;
	}

	CubeStateG1 getStateFromCube(CubeIndexModel& cube) {
		CubeStateG1 stateG1;

		stateG1.cornerOri = cubeIndexer.getCornerOrientationIndex(cube);
		
		int UDSlice = cubeIndexer.getUDSliceIndex(cube);
		int edgeOrientation = cubeIndexer.getEdgeOrientationIndex(cube);
		stateG1.flipUDSlice = edgeOrientation + UDSlice * 2048;

		stateG1.cornerPerm = cubeIndexer.getCornerPermIndex(cube);

		stateG1.UDSliceSort = cubeIndexer.getUDSliceSortIndex(cube);
		stateG1.USliceSort = cubeIndexer.getUSliceSortIndex(cube);
		stateG1.DSliceSort = cubeIndexer.getDSliceSortIndex(cube);

		return stateG1;
	}

	CubeStateG2 getG2StateFromG1State(CubeStateG1& stateG1) {
		CubeStateG2 stateG2;
		
		stateG2.cornerPerm = stateG1.cornerPerm;
		stateG2.edgeMPerm = databaseG2Controller.getEdgeMPerm(stateG1.UDSliceSort);
		stateG2.edgeRPerm = databaseG2Controller.getEdgeRPerm(stateG1.USliceSort, stateG1.DSliceSort);

		return stateG2;
	}

	std::vector<int> kocimbeaTwoPhase(CubeIndexModel& cube, int timeBlock) {
		typedef priority_queue<PrioritizedMoveG1, vector<PrioritizedMoveG1>, greater<PrioritizedMoveG1>> moveQueue_t;

		SolutionSimplifier solutionSimplifier;
		MovePruner movePruner;
		MoveStore moveStore;
		CubeSolver cubeSolver;

		std::vector<int> solution;

		std::stack<NodeG1> nodeStack;
		NodeG1 curNode;
		std::array<int, 50> moves = { (int)0xFF };
		int bound = 0;
		CubeStateG1 startingNode = getStateFromCube(cube);
		int nextBound = databaseG1Controller.getNumMoves(startingNode.flipUDSlice, startingNode.cornerOri);

		int foundSolutionLength = 1e9;
		bool solved = false;

		if (nextBound == 0) {
			CubeStateG2 stateG2 = getG2StateFromG1State(startingNode);
			solution = solveG2WithBlock(stateG2, 1e9);
			std::cout << "Found optimal solution with " << solution.size() << " moves!\n";
			solved = true;
		}

		auto startTime = chrono::high_resolution_clock::now();

		while (!solved &&
			chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count() < timeBlock) {

			if (nodeStack.empty()) {

				nodeStack.push({ startingNode, (int)0xFF, 0 });

				if (nextBound == 0 || nextBound == 0xFF)
					break;

				bound = nextBound;
				cout << "Searching depth " << bound << "\n";
				nextBound = 0xFF;
			}

			curNode = nodeStack.top();
			nodeStack.pop();

			moves.at(curNode.depth) = (int)0xFF;

			if (curNode.depth != 0)
				moves[curNode.depth - 1] = curNode.move;

			if (curNode.depth == bound && databaseG1Controller.getNumMoves(curNode.cube.flipUDSlice, curNode.cube.cornerOri) == 0) {
				
				int g2EstMoves = databaseG2Controller.getNumCornPermMoves(curNode.cube.cornerPerm);

				if (curNode.depth + g2EstMoves < foundSolutionLength) {	
					
					CubeStateG2 stateG2 = getG2StateFromG1State(curNode.cube);
					std::vector<int> solutionG2 = solveG2WithBlock(stateG2, foundSolutionLength - curNode.depth);

					if (solutionG2.empty()) {
						solution.clear();
						for (int i = 0; i < moves.size() && (int)moves.at(i) != 0xFF; i++)
							solution.push_back(moves.at(i));
						cout << "Found optimal solution with " << solution.size() << " moves!\n";
						break;
					}

					if (curNode.depth + solutionG2.size() -
						solutionSimplifier.reductionDifference(curNode.move, solutionG2.front()) < foundSolutionLength) {

						solution.clear();
						for (int i = 0; i < moves.size() && (int)moves.at(i) != 0xFF; i++)
							solution.push_back(moves.at(i));

						solution.insert(solution.end(), solutionG2.begin(), solutionG2.end());
						solutionSimplifier.simplifySolution(solution);

						foundSolutionLength = solution.size();
						cout << "Found solution with " << foundSolutionLength << " moves!\n";
					}
				}
			}

			else {

				moveQueue_t successors;

				for (int move = 0; move < 18; move++) {

					if (curNode.depth == 0 || !movePruner.pruneMove(move, curNode.move)) {

						CubeStateG1 newState;
						newState.flipUDSlice = moveController.updateFlipUDSlice(curNode.cube.flipUDSlice, move);
						newState.cornerOri = moveController.updateCornerOrientaion(curNode.cube.cornerOri, move);
						newState.cornerPerm = moveController.updateCornerPerm(curNode.cube.cornerPerm, move);
						newState.UDSliceSort = moveController.updateUDSliceSorted(curNode.cube.UDSliceSort, move);
						newState.USliceSort = moveController.updateUSliceSorted(curNode.cube.USliceSort, move);
						newState.DSliceSort = moveController.updateDSliceSorted(curNode.cube.DSliceSort, move);

						int estSuccMoves = curNode.depth + 1 + databaseG1Controller.getNumMoves(newState.flipUDSlice, newState.cornerOri);

						if (estSuccMoves <= bound)
							successors.push({ newState, move, estSuccMoves });
						else
							nextBound = estSuccMoves;
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
		
		moveStore.printMoveSequence(solution);
		cout << "\n";
		return solution;

	}


	std::vector<int> solveG2WithBlock(CubeStateG2& startingNode, int depthBlock) {
		typedef priority_queue<PrioritizedMoveG2, vector<PrioritizedMoveG2>, greater<PrioritizedMoveG2>> moveQueue_t;

		MovePruner movePruner;

		std::stack<NodeG2> nodeStack;
		NodeG2 curNode;
		std::array<int, 50> moves = { (int)0xFF };
		int bound = 0;

		int nextBound = databaseG2Controller.getNumMoves(
			startingNode.cornerPerm,
			startingNode.edgeMPerm,
			startingNode.edgeRPerm
		);

		bool solved = false;

		if (nextBound == 0) {
			solved = true;
		}

		while (!solved) {
			if (nodeStack.empty()) {
				nodeStack.push({ startingNode, (int)0xFF, 0 });

				if (nextBound == 0 || nextBound == 0xFF)
					cout << "Hmm";

				bound = nextBound;
				nextBound = 0xFF;
			}

			if (bound >= depthBlock)
				return vector<int>(bound, -1);

			curNode = nodeStack.top();
			nodeStack.pop();

			moves.at(curNode.depth) = (int)0xFF;

			if (curNode.depth != 0)
				moves[curNode.depth - 1] = curNode.move;

			if (curNode.depth == bound) {
				if (databaseG2Controller.getNumMoves(curNode.cube.cornerPerm, curNode.cube.edgeMPerm, curNode.cube.edgeRPerm) == 0)
					solved = true;
			}
			else {
				moveQueue_t successors;

				for (int move = 0; move < 18; move++) {
					if (availableMovesG2 & (1 << move)) {
						
						if (curNode.depth == 0 || !movePruner.pruneMove(move, curNode.move)) {
							CubeStateG2 newState;
							newState.cornerPerm = moveController.updateCornerPerm(curNode.cube.cornerPerm, move);
							newState.edgeMPerm = moveController.updateEdgeMPerm(curNode.cube.edgeMPerm, move);
							newState.edgeRPerm = moveController.updateEdgeRPerm(curNode.cube.edgeRPerm, move);

							int estSuccMoves = curNode.depth + 1 + databaseG2Controller.getNumMoves(
								newState.cornerPerm,
								newState.edgeMPerm,
								newState.edgeRPerm);

							if (estSuccMoves <= bound)
								successors.push({ newState, move, estSuccMoves });
							else
								nextBound = estSuccMoves;
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
		for (int i = 0; i < moves.size() && (int)moves.at(i) != 0xFF; i++) {
			solution.push_back(moves.at(i));
		}
		
		return solution;
	}

};

#endif


//
// All data for recurrent Kociemba G1 with all different time-blocks
// 
//----------------------------------------------------------------------------
// 
// 10 milliseconds time-block
// 
// Correct symmetries:
// 18: 
// 19: ***				->  3 ->  9%
// 20: *********		->  9 -> 28%
// 21: **************	-> 14 -> 44%
// 22: ******			->  6 -> 19%
// 23: 
// 
// Average solution length -> 20.7
// 
// Correct symmetries + no indexModels:
// 18: 
// 19: *****		->  5 -> 19%
// 20: ***********	-> 11 -> 42%
// 21: ********		->  8 -> 31%
// 22: **			->  2 -> 8%
// 23: 
// 
// Average solution length -> 20.2
// 
//----------------------------------------------------------------------------
// 
// 100 milliseconds time-block
// 
// Correct symmetries:
// 18: 
// 19: *******		->  7 -> 27%
// 20: ***********	-> 11 -> 42%
// 21: ********		->  8 -> 31%
// 22: 
// 
// Average solution length -> 20.0
// 
// Correct symmetries + no indexModels:
// 16: 
// 17: *			->  1 -> 4%
// 18: ***			->  3 -> 11%
// 19: **********	-> 10 -> 39%
// 20: *******		->  7 -> 27%
// 21: ****			->  4 -> 15%
// 22: *			->  1 -> 4%
// 23: 
// 
// Average solution length -> 19.5
// 
//----------------------------------------------------------------------------
// 
// 1 second time-block
// 
// Incorrect symmetries:
// 20:
// 21: ******** -> 8 -> 50%
// 22: ******** -> 8 -> 50%
// 23:
//
// Average solution length -> 21.5
// 
// Correct symmetries:
// 18:
// 19: *******		->  7 -> 41%
// 20: **********	-> 10 -> 59%
// 21:
// 
// Average solution length -> 19.6
// 
// Correct symmetries + no indexModels:
// 18: 
// 19: **************	-> 14 -> 54%
// 20: ************		-> 12 -> 46%
// 21: 
// 
// Average solution length -> 19.4
// 
//----------------------------------------------------------------------------
// 
// 5 seconds time-block
// 
// Incorrect symmetries:
// 17: 
// 18: *		-> 1 ->  6%
// 19: 
// 20: *******	-> 7 -> 41%
// 21: *******	-> 7 -> 41%
// 22: *		-> 1 ->  6%
// 23: *		-> 1 ->  6%
// 24: 
// 
// Average solution length -> 20.6
// 
// Correct symmetries:
// 16:
// 17: *			-> 1 -> 5%
// 18: *			-> 1 -> 5%
// 19: ********		-> 8 -> 42%
// 20: *********	-> 9 -> 48%
// 21: 
// 
// Average solution length -> 19.3
// 
// Correct symmetries + no indexModels:
// 16: 
// 17: *				->  1 -> 4%
// 18: ******			->  6 -> 21%
// 19: ****************	-> 16 -> 57%
// 20: *****			->  5 -> 18%
// 21: 
// 
// Average solution length -> 18.9
// 
//----------------------------------------------------------------------------
// 
// 10 seconds time-block 
// 
// Incorrect symmetries:
// 17: 
// 18: **		-> 2 -> 12%
// 19: 
// 20: *******	-> 7 -> 44%
// 21: ******	-> 6 -> 38%
// 22: *		-> 1 ->  6%
// 23: 
// 
// Average solution length -> 20.2
//
//----------------------------------------------------------------------------
// 
// 30 seconds time-block 
// 
// Incorrect symmetries:
// 17: 
// 18: *		-> 1 -> 6%
// 19: ******	-> 6 -> 38%
// 20: ******	-> 6 -> 38%
// 21: ***		-> 3 -> 18%
// 22: 
// 
// Average solution length -> 19.7
// 
// Correct symmetries:
// 15: 
// 16: 
// 17: 
// 18: ******	-> 6 -> 33%
// 19: ********	-> 8 -> 45%
// 20: ****		-> 4 -> 22%
// 21: 
// 
// Average solution length -> 18.9
//
//----------------------------------------------------------------------------
// 
// 60 seconds time-block 
// 
// Incorrect symmetries:
// 18: 
// 19: *****	-> 5 -> 33%
// 20: *******	-> 7 -> 47%
// 21: ***		-> 3 -> 20%
// 22: 
// 
// Average solution length -> 19.9
//
//----------------------------------------------------------------------------
// 
// 300 seconds time-block 
// 
// Incorrect symmetries:
// 15: 
// 16: **
// 17: 
// 18: **
// 19: *
// 20: ***
// 21: *
// 22: 
// 
// Average solution length -> 18.6
// 
//----------------------------------------------------------------------------
// 
// 
//
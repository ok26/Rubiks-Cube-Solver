#ifndef KOCIEMBA_SOLVER_H
#define KOCIEMBA_SOLVER_H

#include "../Model/cubeIndexModel.h"
#include "../Database/Kociemba/pDatabaseG1.h"
#include "../Database/Kociemba/pDatabaseG2.h"
#include "cubeSolver.h"

#include <iostream>
#include <queue>
#include <vector>
#include <stack>

using namespace std;

class KociembaSolver {
public:
	int availableMoves[2] = { 262143, 151524 };
	PatternDatabase* korfDatabaseG1;
	PatternDatabase* korfDatabaseG2;

	KociembaPatternDatabaseG1 dKociembaDatabaseG1 = KociembaPatternDatabaseG1();
	KociembaPatternDatabaseG2 dKociembaDatabaseG2 = KociembaPatternDatabaseG2();

	KociembaSolver() {
		korfDatabaseG1 = &dKociembaDatabaseG1;
		korfDatabaseG2 = &dKociembaDatabaseG2;
	}

	vector<int> solveCubeIDA(CubeIndexModel cube) {
		CubeSolver cubeSolver;
		vector<int> solution;

		vector<int> solutionG1 = cubeSolver.solveIDA(korfDatabaseG1, cube, availableMoves[0]);
		for (int move : solutionG1)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG1.begin(), solutionG1.end());

		vector<int> solutionG2 = cubeSolver.solveIDA(korfDatabaseG2, cube, availableMoves[1]);
		for (int move : solutionG2)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG2.begin(), solutionG2.end());

		cout << "Solved cube in " << solution.size() << "moves!\n";

		return solution;
	}
};

#endif // !KOCIEMBA_SOLVER_H

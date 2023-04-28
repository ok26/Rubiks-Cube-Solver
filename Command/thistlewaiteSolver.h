#ifndef THISTLEWAITE_SOLVER_H
#define THISTLEWAITE_SOLVER_H

#include "../Model/cubeIndexModel.h"
#include "../Database/Thistlewaite/pDatabaseG1.h"
#include "../Database/Thistlewaite/pDatabaseG2.h"
#include "../Database/Thistlewaite/pDatabaseG3.h"
#include "../Database/Thistlewaite/pDatabaseG4.h"
#include "../Database/moveStore.h"
#include "simplifySolution.h"
#include "cubeSolver.h"

#include <iostream>
#include <queue>
#include <vector>
#include <stack>
#include <functional>

using namespace std;

class ThistlewaiteSolver {

	int availableMoves[4] = { 262143, 151551, 151524, 149796 };
	PatternDatabase* pDatabaseG1;
	PatternDatabase* pDatabaseG2;
	PatternDatabase* pDatabaseG3;
	PatternDatabase* pDatabaseG4;

public:
	bool tablesInitialized = false;

	ThistlewaiteSolver(bool initializeTablesOnCreate = true) {

		if (initializeTablesOnCreate) 
			initializeTables();
	}

	void initializeTables() {
		PatternDatabaseG1 dDatabaseG1 = PatternDatabaseG1();
		PatternDatabaseG2 dDatabaseG2 = PatternDatabaseG2();
		PatternDatabaseG3 dDatabaseG3 = PatternDatabaseG3();
		PatternDatabaseG4 dDatabaseG4 = PatternDatabaseG4();

		dDatabaseG1.readThread.join();
		dDatabaseG2.readThread.join();
		dDatabaseG3.readThread.join();
		dDatabaseG4.readThread.join();

		pDatabaseG1 = &dDatabaseG1;
		pDatabaseG2 = &dDatabaseG2;
		pDatabaseG3 = &dDatabaseG3;
		pDatabaseG4 = &dDatabaseG4;

		tablesInitialized = true;
	}

	vector<int> solveCubeBFS(CubeIndexModel cube) {
		CubeSolver cubeSolver;
		SolutionSimplifier solutionSimplifier;
		MoveStore moveStore;
		vector<int> solution;
		
		vector<int> solutionG1 = cubeSolver.solveBFS(pDatabaseG1, cube, availableMoves[0]);
		for (int move : solutionG1)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG1.begin(), solutionG1.end());
		
		vector<int> solutionG2 = cubeSolver.solveBFS(pDatabaseG2, cube, availableMoves[1]);
		for (int move : solutionG2)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG2.begin(), solutionG2.end());
		
		vector<int> solutionG3 = cubeSolver.solveBFS(pDatabaseG3, cube, availableMoves[2]);
		for (int move : solutionG3)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG3.begin(), solutionG3.end());
		
		vector<int> solutionG4 = cubeSolver.solveBFS(pDatabaseG4, cube, availableMoves[3]);
		for (int move : solutionG4)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG4.begin(), solutionG4.end());
		
		solutionSimplifier.simplifySolution(solution);
		cout << "Found solution with " << solution.size() << " moves!\n";
		moveStore.printMoveSequence(solution);
		cout << "\n";
		return solution;
	}


	vector<int> solveCubeIDA(CubeIndexModel cube) {
		CubeSolver cubeSolver;
		SolutionSimplifier solutionSimplifier;
		MoveStore moveStore;
		vector<int> solution;

		vector<int> solutionG1 = cubeSolver.solveIDA(pDatabaseG1, cube, availableMoves[0]);
		for (int move : solutionG1)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG1.begin(), solutionG1.end());
	
		vector<int> solutionG2 = cubeSolver.solveIDA(pDatabaseG2, cube, availableMoves[1]);
		for (int move : solutionG2)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG2.begin(), solutionG2.end());
		
		vector<int> solutionG3 = cubeSolver.solveIDA(pDatabaseG3, cube, availableMoves[2]);
		for (int move : solutionG3)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG3.begin(), solutionG3.end());
		
		vector<int> solutionG4 = cubeSolver.solveIDA(pDatabaseG4, cube, availableMoves[3]);
		for (int move : solutionG4)
			cube.doMove(move);
		solution.insert(solution.end(), solutionG4.begin(), solutionG4.end());

		solutionSimplifier.simplifySolution(solution);
		cout << "Found solution with " << solution.size() << " moves!\n";
		moveStore.printMoveSequence(solution);
		cout << "\n";
		return solution;
	}
};

#endif

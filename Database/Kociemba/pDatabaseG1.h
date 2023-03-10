#ifndef KORF_G1_DATABASE_H
#define KORF_G1_DATABASE_H

#include "../patternDatabase.h"
#include "pDatabaseG1Corners.h"
#include "pDatabaseG1Edges.h"

using namespace std;

class KociembaPatternDatabaseG1 : public PatternDatabase, public PatternDatabaseG1Corners, public PatternDatabaseG1Edges {
public:

	KociembaPatternDatabaseG1() : PatternDatabaseG1Corners(), PatternDatabaseG1Edges() {
		PatternDatabaseG1Corners::readThread.join();
		PatternDatabaseG1Edges::readThread.join();
	}

	int getNumMoves(CubeIndexModel cube) {
		int test1 = PatternDatabaseG1Edges::getNumMoves(cube);
		int test2 = PatternDatabaseG1Corners::getNumMoves(cube);
		return max(test1, test2);
	}
};

#endif
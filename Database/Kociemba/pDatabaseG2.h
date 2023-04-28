#ifndef PATTERN_DATABASE_G2_KOCIEMBA
#define PATTERN_DATABASE_G2_KOCIEMBA

#include "../patternDatabase.h"
#include "pDatabaseG2Corners.h"
#include "pDatabaseG2Edges.h"

using namespace std;

class KociembaPatternDatabaseG2 : public PatternDatabase, public PatternDatabaseG2Corners, public PatternDatabaseG2Edges {
public:

	KociembaPatternDatabaseG2() : PatternDatabaseG2Corners(), PatternDatabaseG2Edges() {
		PatternDatabaseG2Corners::readThread.join();
		PatternDatabaseG2Edges::readThread.join();
	}

	int getNumMoves(CubeIndexModel cube) {
		int test1 = PatternDatabaseG2Edges::getNumMoves(cube);
		int test2 = PatternDatabaseG2Corners::getNumMoves(cube);
		return max(test1, test2);
	}
};

#endif 
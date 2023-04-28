#ifndef G1_MOVE_CONTROLLER
#define G1_MOVE_CONTROLLER

#include <array>

class MoveController {

	std::array<std::array<int, 18>, 2187>		cornerOriMoveTable;
	std::array<std::array<int, 18>, 1013760>	edgeUDMoveTable;
	std::array<std::array<int, 18>, 40320>		cornerPermMoveTable;
	std::array<std::array<int, 18>, 40320>		edgeRPermMoveTable;
	std::array<std::array<int, 18>, 24>			edgeMPermMoveTable;
	std::array<std::array<int, 18>, 11880>		UDSliceSortMoveTable;
	std::array<std::array<int, 18>, 11880>		USliceSortMoveTable;
	std::array<std::array<int, 18>, 11880>		DSliceSortMoveTable;

	int availableMovesG2 = 151524;

	ReverseCubeIndexer reverseCubeIndexer;
	CubeIndexer cubeIndexer;

public:

	void initializeTables() {
		generateCornerOriTable();
		generateEdgeUDTable();
		generateCornerPermTable();
		generateUDSliceSortTable();
		generateUSliceSortTable();
		generateDSliceSortTable();
		generateEdgeMPermTable();
		generateEdgeRPermTable();
	}

	void generateCornerOriTable() {

		for (int i = 0; i < cornerOriMoveTable.size(); i++) {
			CubeIndexModel cube;
			reverseCubeIndexer.updateCubeCornerOrientation(cube, i);
			for (int move = 0; move < 18; move++) {
				cube.doMove(move);
				cornerOriMoveTable[i][move] = cubeIndexer.getCornerOrientationIndex(cube);
				cube.doReverseMove(move);
			}
		}
		std::cout << "Finished generating CornerOrientation move table\n";
	}

	void generateEdgeUDTable() {

		for (int i = 0; i < edgeUDMoveTable.size(); i++) {
			CubeIndexModel cube;
			reverseCubeIndexer.updateCubeEdgeMComb(cube, (i / 2048));
			reverseCubeIndexer.updateCubeEdgeOrientaion(cube, (i % 2048));
			for (int move = 0; move < 18; move++) {
				cube.doMove(move);
				int UDSliceIndex = cubeIndexer.getUDSliceIndex(cube);
				int edgeOrientaionIndex = cubeIndexer.getEdgeOrientationIndex(cube);
				edgeUDMoveTable[i][move] = edgeOrientaionIndex + UDSliceIndex * 2048;
				cube.doReverseMove(move);
			}
		}
		std::cout << "Finished generating EdgeUD move table\n";
	}

	void generateCornerPermTable() {

		for (int i = 0; i < cornerPermMoveTable.size(); i++) {
			CubeIndexModel cube;
			reverseCubeIndexer.updateCubeCornerPerm(cube, i);
			for (int move = 0; move < 18; move++) {
				cube.doMove(move);
				cornerPermMoveTable[i][move] = cubeIndexer.getCornerPermIndex(cube);
				cube.doReverseMove(move);
			}
		}
		std::cout << "Finished generating CornerPerm move table\n";
	}

	void generateEdgeRPermTable() {

		for (int i = 0; i < edgeRPermMoveTable.size(); i++) {
			CubeIndexModel cube;
			reverseCubeIndexer.updateCubeEdgeRPerm(cube, i);
			for (int move = 0; move < 18; move++) {
				if ((1 << move) & availableMovesG2) {
					cube.doMove(move);
					edgeRPermMoveTable[i][move] = cubeIndexer.getEdgeRPermIndex(cube);
					cube.doReverseMove(move);
				}
			}
		}
		std::cout << "Finished generating EdgeRPerm move table\n";
	}

	void generateEdgeMPermTable() {

		for (int i = 0; i < edgeMPermMoveTable.size(); i++) {
			CubeIndexModel cube;
			reverseCubeIndexer.updateCubeEdgeMPerm(cube, i);
			for (int move = 0; move < 18; move++) {
				if ((1 << move) & availableMovesG2) {
					cube.doMove(move);
					edgeMPermMoveTable[i][move] = cubeIndexer.getEdgeMPermIndex(cube);
					cube.doReverseMove(move);
				}
			}
		}
		std::cout << "Finished generating EdgeMPerm move table\n";
	}

	void generateUDSliceSortTable() {

		for (int i = 0; i < UDSliceSortMoveTable.size(); i++) {
			CubeIndexModel cube(blank = true);
			reverseCubeIndexer.updateCubeUDSlice(cube, i);
			for (int move = 0; move < 18; move++) {
				cube.doMove(move);
				UDSliceSortMoveTable[i][move] = cubeIndexer.getUDSliceSortIndex(cube);
				cube.doReverseMove(move);
			}
		}
		std::cout << "Finished generating UDSliceSort move table\n";
	}

	void generateUSliceSortTable() {

		for (int i = 0; i < USliceSortMoveTable.size(); i++) {
			CubeIndexModel cube(blank = true);
			reverseCubeIndexer.updateCubeUSlice(cube, i);
			for (int move = 0; move < 18; move++) {
				cube.doMove(move);
				USliceSortMoveTable[i][move] = cubeIndexer.getUSliceSortIndex(cube);
				cube.doReverseMove(move);
			}
		}
		std::cout << "Finished generating USliceSort move table\n";
	}

	void generateDSliceSortTable() {

		for (int i = 0; i < DSliceSortMoveTable.size(); i++) {
			CubeIndexModel cube(blank = true);
			reverseCubeIndexer.updateCubeDSlice(cube, i);
			for (int move = 0; move < 18; move++) {
				cube.doMove(move);
				DSliceSortMoveTable[i][move] = cubeIndexer.getDSliceSortIndex(cube);
				cube.doReverseMove(move);
			}
		}
		std::cout << "Finished generating DSliceSort move table\n";
	}

	int updateFlipUDSlice(int flipUDSlice, int move) {
		return edgeUDMoveTable[flipUDSlice][move];
	}

	int updateCornerOrientaion(int cornerOrientaion, int move) {
		return cornerOriMoveTable[cornerOrientaion][move];
	}

	int updateUDSliceSorted(int UDSliceSort, int move) {
		return UDSliceSortMoveTable[UDSliceSort][move];
	}

	int updateUSliceSorted(int USliceSort, int move) {
		return USliceSortMoveTable[USliceSort][move];
	}

	int updateDSliceSorted(int DSliceSort, int move) {
		return DSliceSortMoveTable[DSliceSort][move];
	}

	int updateCornerPerm(int cornerPerm, int move) {
		return cornerPermMoveTable[cornerPerm][move];
	}

	int updateEdgeRPerm(int edgeRPerm, int move) {
		return edgeRPermMoveTable[edgeRPerm][move];
	}
	
	int updateEdgeMPerm(int edgeMPerm, int move) {
		return edgeMPermMoveTable[edgeMPerm][move];
	}

};

#endif
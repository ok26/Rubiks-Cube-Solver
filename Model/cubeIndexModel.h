#ifndef CUBE_INDEX_H
#define CUBE_INDEX_H

#include <algorithm>
#include <array>
#include <vector>
#include <iostream>

struct Cubie {
	int index;
	int orientation;
};

enum indexMove {R, Rp, R2, L, Lp, L2, U, Up, U2, D, Dp, D2, F, Fp, F2, B, Bp, B2};

std::array<std::array<int, 8>, 6> affectedCubies = {
	std::array<int, 8>{0, 1, 2, 3, 0, 1, 2, 3},
	{2, 4, 8, 5, 2, 7, 4, 3},
	{1, 7, 11, 4, 1, 6, 7, 2},
	{3, 5, 9, 6, 3, 4, 5, 0},
	{0, 6, 10, 7, 0, 5, 6, 1},
	{8, 11, 10, 9, 7, 6, 5, 4}
};

std::array<int, 6> faceIndices = { 2, 3, 0, 5, 1, 4 };

class CubeIndexModel {
public:
	std::array<Cubie, 12> edges;
	std::array<Cubie, 8> corners;
	std::array<int, 6> centers = {0, 1, 2, 3, 4, 5};

	CubeIndexModel(bool blank = false) {
		for (int i = 0; i < 12; i++) {
			edges[i].index = blank ? -1 : i;
			edges[i].orientation = 0;
		}

		for (int i = 0; i < 8; i++) {
			corners[i].index = blank ? -1 : i;
			corners[i].orientation = 0;
		}
	}

	CubeIndexModel(const CubeIndexModel& cubeCopy) {
		for (int i = 0; i < 12; i++) {
			edges[i] = cubeCopy.edges[i];
		}

		for (int i = 0; i < 8; i++) {
			corners[i] = cubeCopy.corners[i];
		}
	}

	bool isSolved() {
		for (int i = 0; i < 12; i++)
			if (edges[i].index != i || edges[i].orientation != 0)
				return false;

		for (int i = 0; i < 8; i++)
			if (corners[i].index != i || corners[i].orientation != 0)
				return false;

		return true;
	}

	void printData() {
		for (int i = 0; i < 8; i++)
			std::cout << corners[i].index << " " << corners[i].orientation << "\n";

		std::cout << "\n";

		for (int i = 0; i < 12; i++)
			std::cout << edges[i].index << " " << edges[i].orientation << "\n";
	}

	void updateEdgeOrientation(int affectedFace) {
		for (int i = 0; i < 4; i++) {
			edges[affectedCubies[affectedFace][i]].orientation ^= 1;
		}
	}

	void updateCubieIndex(int affectedFace, bool inverse, bool doubleMove) {
		if (doubleMove) {
			std::swap(edges[affectedCubies[affectedFace][0]], edges[affectedCubies[affectedFace][2]]);
			std::swap(edges[affectedCubies[affectedFace][1]], edges[affectedCubies[affectedFace][3]]);

			std::swap(corners[affectedCubies[affectedFace][4]], corners[affectedCubies[affectedFace][6]]);
			std::swap(corners[affectedCubies[affectedFace][5]], corners[affectedCubies[affectedFace][7]]);
			return;
		}

		Cubie hold;

		if (inverse) {
			hold = edges[affectedCubies[affectedFace][0]];
			edges[affectedCubies[affectedFace][0]] = edges[affectedCubies[affectedFace][1]];
			edges[affectedCubies[affectedFace][1]] = edges[affectedCubies[affectedFace][2]];
			edges[affectedCubies[affectedFace][2]] = edges[affectedCubies[affectedFace][3]];
			edges[affectedCubies[affectedFace][3]] = hold;

			hold = corners[affectedCubies[affectedFace][4]];
			corners[affectedCubies[affectedFace][4]] = corners[affectedCubies[affectedFace][5]];
			corners[affectedCubies[affectedFace][5]] = corners[affectedCubies[affectedFace][6]];
			corners[affectedCubies[affectedFace][6]] = corners[affectedCubies[affectedFace][7]];
			corners[affectedCubies[affectedFace][7]] = hold;
		}
		else {
			hold = edges[affectedCubies[affectedFace][0]];
			edges[affectedCubies[affectedFace][0]] = edges[affectedCubies[affectedFace][3]];
			edges[affectedCubies[affectedFace][3]] = edges[affectedCubies[affectedFace][2]];
			edges[affectedCubies[affectedFace][2]] = edges[affectedCubies[affectedFace][1]];
			edges[affectedCubies[affectedFace][1]] = hold;

			hold = corners[affectedCubies[affectedFace][4]];
			corners[affectedCubies[affectedFace][4]] = corners[affectedCubies[affectedFace][7]];
			corners[affectedCubies[affectedFace][7]] = corners[affectedCubies[affectedFace][6]];
			corners[affectedCubies[affectedFace][6]] = corners[affectedCubies[affectedFace][5]];
			corners[affectedCubies[affectedFace][5]] = hold;
		}
	}

	void fullRotation(int axis, bool inverse) {
		int hold;
		if (axis == 0) {
			if (inverse) {
				hold = centers[0];
				centers[0] = centers[1];
				centers[1] = centers[5];
				centers[5] = centers[4];
				centers[4] = hold;
			}
			else {
				hold = centers[0];
				centers[0] = centers[4];
				centers[4] = centers[5];
				centers[5] = centers[1];
				centers[1] = hold;
			}
		}
		else if (axis == 1) {
			if (inverse) {
				hold = centers[1];
				centers[1] = centers[2];
				centers[2] = centers[4];
				centers[4] = centers[3];
				centers[3] = hold;
			}
			else {
				hold = centers[1];
				centers[1] = centers[3];
				centers[3] = centers[4];
				centers[4] = centers[2];
				centers[2] = hold;
			}
		}
	}

	void doMove(int indexMove) {
		int affectedFace = centers[ faceIndices[indexMove / 3] ];
		bool inverse = indexMove % 3 == 1;
		bool doubleMove = indexMove % 3 == 2;
		updateCubieIndex(affectedFace, inverse, doubleMove);
		if (!doubleMove && affectedFace > 0 && affectedFace < 5) {
			int& c1Ori = corners[affectedCubies[affectedFace][4]].orientation;
			c1Ori = (c1Ori + 1) % 3;
			int& c2Ori = corners[affectedCubies[affectedFace][5]].orientation;
			c2Ori = (c2Ori + 2) % 3;
			int& c3Ori = corners[affectedCubies[affectedFace][6]].orientation;
			c3Ori = (c3Ori + 1) % 3;
			int& c4Ori = corners[affectedCubies[affectedFace][7]].orientation;
			c4Ori = (c4Ori + 2) % 3;

			if (affectedFace == 1 || affectedFace == 4)
				for (int i = 0; i < 4; i++)
					edges[affectedCubies[affectedFace][i]].orientation ^= 1;
		}
	}

	void doReverseMove(int move) {
		if ((move % 3) == 2)
			doMove(move);
		else if ((move % 3) == 1)
			doMove(move - 1);
		else
			doMove(move + 1);
	}
};

#endif
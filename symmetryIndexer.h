#ifndef SYMMETRY_INDEXER_H
#define SYMMETRY_INDEXER_H

#include "../Model/cubeIndexModel.h"

std::array<std::array<int, 20>, 16> equivalentSymmetryCubies{
	std::array<int, 20>	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7},
						{1, 2, 3, 0, 5, 6, 7, 4, 9, 10, 11, 8, 1, 2, 3, 0, 5, 6, 7, 4},
						{2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 2, 3, 0, 1, 6, 7, 4, 5},
						{3, 0, 1, 2, 7, 4, 5, 6, 11, 8, 9, 10, 3, 0, 1, 2, 7, 4, 5, 6},
						{8, 11, 10, 9, 7, 6, 5, 4, 0, 3, 2, 1, 4, 7, 6, 5, 0, 3, 2, 1},
						{11, 10, 9, 8, 6, 5, 4, 7, 3, 2, 1, 0, 7, 6, 5, 4, 3, 2, 1, 0},
						{10, 9, 8, 11, 5, 4, 7, 6, 2, 1, 0, 3, 6, 5, 4, 7, 2, 1, 0, 3},
						{9, 8, 11, 10, 4, 7, 6, 5, 1, 0, 3, 2, 5, 4, 7, 6, 1, 0, 3, 2},

						{0, 3, 2, 1, 5, 4, 7, 6, 8, 11, 10, 9, 1, 0, 3, 2, 7, 6, 5, 4},
						{3, 2, 1, 0, 4, 7, 6, 5, 11, 10, 9, 8, 0, 3, 2, 1, 6, 5, 4, 7},
						{2, 1, 0, 3, 7, 6, 5, 4, 10, 9, 8, 11, 3, 2, 1, 0, 5, 4, 7, 6},
						{1, 0, 3, 2, 6, 5, 4, 7, 9, 8, 11, 10, 2, 1, 0, 3, 4, 7, 6, 5},
						{8, 9, 10, 11, 6, 7, 4, 5, 0, 1, 2, 3, 7, 4, 5, 6, 1, 2, 3, 0},
						{9, 10, 11, 8, 7, 4, 5, 6, 1, 2, 3, 0, 4, 5, 6, 7, 2, 3, 0, 1},
						{10, 11, 8, 9, 4, 5, 6, 7, 2, 3, 0, 1, 5, 6, 7, 4, 3, 0, 1, 2},
						{11, 8, 9, 10, 5, 6, 7, 4, 3, 0, 1, 2, 6, 7, 4, 5, 0, 1, 2, 3},
};
std::array<std::array<int, 20>, 2> tripleSearchSymmetries{
	std::array<int, 20> {7, 11, 4, 1, 8, 2, 0, 10, 5, 3, 6, 9, 1, 6, 7, 2, 3, 0, 5, 4},
						{10, 7, 0, 6, 1, 3, 9, 11, 2, 5, 8, 4, 5, 6, 1, 0, 3, 4, 7, 2}
};
std::array<int, 12> edgeOriOffset{ 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1 };

array<int, 16> reverseEquivalences = { 0,  3,  2,  1,  4,  5,  6,  7,  8, 9, 10,  11, 12, 15, 14, 13 };

class SymmetryIndexer {
public:

	CubeIndexModel overlaySymmetry(const CubeIndexModel& cube, int symmetryCoord) {
		std::array<int, 20>& symmetryCubieIndeces = equivalentSymmetryCubies[symmetryCoord];
		bool oddX1Symmetry = (symmetryCoord % 4) % 2;
		CubeIndexModel cubeSymmetry;

		for (int i = 0; i < 12; i++) {
			Cubie& edge = cubeSymmetry.edges[symmetryCubieIndeces.at(i)];

			edge.index = symmetryCubieIndeces.at(cube.edges[i].index);
			edge.orientation = cube.edges[i].orientation;

			if (((edge.index / 4 == 1 && (edge.index / 4) != (symmetryCubieIndeces.at(i) / 4)) ||
				((edge.index / 4) != 1 && (symmetryCubieIndeces.at(i) / 4) == 1)) && oddX1Symmetry)
				edge.orientation ^= 1;
		}
		for (int i = 12; i < 20; i++) {
			Cubie& corner = cubeSymmetry.corners[symmetryCubieIndeces.at(i)];

			corner.index = symmetryCubieIndeces.at(cube.corners[i - 12].index + 12);
			corner.orientation = cube.corners[i - 12].orientation;

			if (symmetryCoord >= 8)
				corner.orientation = (3 - corner.orientation) % 3;
		}
		return cubeSymmetry;
	}

	CubeIndexModel overlayTripleSearchSymmetry(const CubeIndexModel& cube, int symmetryCoord) {
		std::array<int, 20>& symmetryCubieIndeces = tripleSearchSymmetries[symmetryCoord];
		CubeIndexModel cubeSymmetry;

		for (int i = 0; i < 12; i++) {
			Cubie& edge = cubeSymmetry.edges[symmetryCubieIndeces.at(i)];

			edge.index = symmetryCubieIndeces.at(cube.edges[i].index);
			edge.orientation = cube.edges[i].orientation;

			if (symmetryCoord == 1 && edgeOriOffset[edge.index] != edgeOriOffset[symmetryCubieIndeces.at(i)])
				edge.orientation ^= 1;
		}
		for (int i = 12; i < 20; i++) {
			Cubie& corner = cubeSymmetry.corners[symmetryCubieIndeces.at(i)];

			corner.index = symmetryCubieIndeces.at(cube.corners[i - 12].index + 12);
			corner.orientation = cube.corners[i - 12].orientation;
			
			if (corner.index != symmetryCubieIndeces.at(i) && (abs(corner.index - symmetryCubieIndeces.at(i)) & 1)) {
				corner.orientation += 2;
				if (symmetryCoord == 0) corner.orientation = (corner.orientation + 3 - (symmetryCubieIndeces.at(i) & 1)) % 3;
				else if (symmetryCoord == 1) corner.orientation = (corner.orientation + 3 - ((symmetryCubieIndeces.at(i) & 1) == 0)) % 3;
			}
		}
		return cubeSymmetry;
	}

	CubeIndexModel backwardsSymmetryOverlay(const CubeIndexModel& cubeSymmetry, int symmetryCoord) {
		return overlaySymmetry(cubeSymmetry, reverseEquivalences[ symmetryCoord ]);
	}
};

#endif
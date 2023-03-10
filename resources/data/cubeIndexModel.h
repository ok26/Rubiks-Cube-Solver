#ifndef CUBE_INDEX_H
#define CUBE_INDEX_H

#include <algorithm>
#include <array>
#include <iostream>

struct Cubie {
	int index;
	int orientation;
};

struct Move {
	int target;
	float duration;
	int axis;
	bool inverse;
};

enum colors {YELLOW, BLUE, RED, GREEN, ORANGE, WHITE};
enum edgePositions {ub, ur, uf, ul, fr, fl, bl, br, df, dl, db, dr};
enum cornerPositions {ulb, urb, urf, ulf, dlf, dlb, drb, drf};
enum indexMove {R, Rp, R2, L, Lp, L2, U, Up, U2, D, Dp, D2, F, Fp, F2, B, Bp, B2};

class CubeIndexModel {
public:
	std::array<Cubie, 12> edges;
	std::array<Cubie, 8> corners;

	CubeIndexModel() {
		for (int i = 0; i < 12; i++) {
			edges[i].index = i;
			edges[i].orientation = 0;
		}

		for (int i = 0; i < 8; i++) {
			corners[i].index = i;
			corners[i].orientation = 0;
		}
	}

	//temporary
	void processMove(Move move) {
		switch (move.target) {
		case 0:
			if (move.inverse)
				doMove(U);
			else
				doMove(Up);
			break;
		case 1:
			if (move.inverse)
				doMove(F);
			else
				doMove(Fp);
			break;
		case 2:
			if (move.inverse)
				doMove(R);
			else
				doMove(Rp);
			break;
		case 3:
			if (move.inverse)
				doMove(Lp);
			else
				doMove(L);
			break;
		case 4:
			if (move.inverse)
				doMove(Bp);
			else
				doMove(B);
			break;
		case 5:
			if (move.inverse)
				doMove(Dp);
			else
				doMove(D);
			break;
		}
	}
	//********************************************************************************************
	//FULLY REFRACTOR THE MOVE TING -------- FUCING INVERSE DOES NOT EVEN MEAN PRIME MOVE -------
	//********************************************************************************************

	void printData() {
		for (int i = 0; i < 12; i++) {
			std::cout << edges[i].index << " " << edges[i].orientation << std::endl;
		}
		std::cout << std::endl;
		for (int i = 0; i < 8; i++) {
			std::cout << corners[i].index << " " << corners[i].orientation << std::endl;
		}
		std::cout << std::endl;
	}

	void updateCornerOrientationX(cornerPositions index) {
		Cubie& corner = corners[index];
		bool evenDistance = (index + corner.index) % 2 == 0;
		if (corner.orientation == 0)
			corner.orientation = evenDistance ? 2 : 1;
		else if (corner.orientation == 1)
			corner.orientation = evenDistance ? 0 : 2;
		else
			corner.orientation = evenDistance ? 1 : 0;
	}

	void updateCornerOrientationZ(cornerPositions index) {
		Cubie& corner = corners[index];
		bool evenDistance = (index + corner.index) % 2 == 0;
		if (corner.orientation == 0)
			corner.orientation = evenDistance ? 1 : 2;
		else if (corner.orientation == 1)
			corner.orientation = evenDistance ? 2 : 0;
		else
			corner.orientation = evenDistance ? 0 : 1;
	}

	void doMove(int indexMove) {
		Cubie hold;
		switch (indexMove) {
		case U:
			hold = edges[ul];
			edges[ul] = edges[uf];
			edges[uf] = edges[ur];
			edges[ur] = edges[ub];
			edges[ub] = hold;

			hold = corners[ulf];
			corners[ulf] = corners[urf];
			corners[urf] = corners[urb];
			corners[urb] = corners[ulb];
			corners[ulb] = hold;
			break;
		case Up:
			hold = edges[ub];
			edges[ub] = edges[ur];
			edges[ur] = edges[uf];
			edges[uf] = edges[ul];
			edges[ul] = hold;

			hold = corners[ulb];
			corners[ulb] = corners[urb];
			corners[urb] = corners[urf];
			corners[urf] = corners[ulf];
			corners[ulf] = hold;
			break;
		case U2:
			std::swap(edges[ub], edges[uf]);
			std::swap(edges[ul], edges[ur]);

			std::swap(corners[ulb], corners[urf]);
			std::swap(corners[urb], corners[ulf]);
			break;
		case L:
			hold = edges[bl];
			edges[bl] = edges[dl];
			edges[dl] = edges[fl];
			edges[fl] = edges[ul];
			edges[ul] = hold;

			hold = corners[dlb];
			corners[dlb] = corners[dlf];
			corners[dlf] = corners[ulf];
			corners[ulf] = corners[ulb];
			corners[ulb] = hold;

			updateCornerOrientationX(dlb);
			updateCornerOrientationX(dlf);
			updateCornerOrientationX(ulf);
			updateCornerOrientationX(ulb);
			break;
		case Lp:
			hold = edges[bl];
			edges[bl] = edges[ul];
			edges[ul] = edges[fl];
			edges[fl] = edges[dl];
			edges[dl] = hold;

			hold = corners[dlb];
			corners[dlb] = corners[ulb];
			corners[ulb] = corners[ulf];
			corners[ulf] = corners[dlf];
			corners[dlf] = hold;

			updateCornerOrientationX(dlb);
			updateCornerOrientationX(dlf);
			updateCornerOrientationX(ulf);
			updateCornerOrientationX(ulb);
			break;
		case L2:
			std::swap(edges[bl], edges[fl]);
			std::swap(edges[ul], edges[dl]);

			std::swap(corners[dlb], corners[ulf]);
			std::swap(corners[ulb], corners[dlf]);
			break;
		case F:
			hold = edges[uf];
			edges[uf] = edges[fl];
			edges[fl] = edges[df];
			edges[df] = edges[fr];
			edges[fr] = hold;

			edges[uf].orientation ^= 1;
			edges[fl].orientation ^= 1;
			edges[df].orientation ^= 1;
			edges[fr].orientation ^= 1;

			hold = corners[ulf];
			corners[ulf] = corners[dlf];
			corners[dlf] = corners[drf];
			corners[drf] = corners[urf];
			corners[urf] = hold;

			updateCornerOrientationZ(ulf);
			updateCornerOrientationZ(dlf);
			updateCornerOrientationZ(drf);
			updateCornerOrientationZ(urf);
			break;
		case Fp:
			hold = edges[uf];
			edges[uf] = edges[fr];
			edges[fr] = edges[df];
			edges[df] = edges[fl];
			edges[fl] = hold;

			edges[uf].orientation ^= 1;
			edges[fl].orientation ^= 1;
			edges[df].orientation ^= 1;
			edges[fr].orientation ^= 1;

			hold = corners[ulf];
			corners[ulf] = corners[urf];
			corners[urf] = corners[drf];
			corners[drf] = corners[dlf];
			corners[dlf] = hold;

			updateCornerOrientationZ(ulf);
			updateCornerOrientationZ(dlf);
			updateCornerOrientationZ(drf);
			updateCornerOrientationZ(urf);
			break;
		case F2:
			std::swap(edges[uf], edges[df]);
			std::swap(edges[fl], edges[fr]);

			std::swap(corners[ulf], corners[drf]);
			std::swap(corners[urf], corners[dlf]);
			break;
		case R:
			hold = edges[br];
			edges[br] = edges[ur];
			edges[ur] = edges[fr];
			edges[fr] = edges[dr];
			edges[dr] = hold;

			hold = corners[drb];
			corners[drb] = corners[urb];
			corners[urb] = corners[urf];
			corners[urf] = corners[drf];
			corners[drf] = hold;

			updateCornerOrientationX(drb);
			updateCornerOrientationX(urb);
			updateCornerOrientationX(urf);
			updateCornerOrientationX(drf);
			break;
		case Rp:
			hold = edges[br];
			edges[br] = edges[dr];
			edges[dr] = edges[fr];
			edges[fr] = edges[ur];
			edges[ur] = hold;

			hold = corners[drb];
			corners[drb] = corners[drf];
			corners[drf] = corners[urf];
			corners[urf] = corners[urb];
			corners[urb] = hold;

			updateCornerOrientationX(drb);
			updateCornerOrientationX(urb);
			updateCornerOrientationX(urf);
			updateCornerOrientationX(drf);
			break;
		case R2:
			std::swap(edges[br], edges[fr]);
			std::swap(edges[ur], edges[dr]);

			std::swap(corners[drb], corners[urf]);
			std::swap(corners[urb], corners[drf]);
			break;
		case B:
			hold = edges[ub];
			edges[ub] = edges[br];
			edges[br] = edges[db];
			edges[db] = edges[bl];
			edges[bl] = hold;

			edges[ub].orientation ^= 1;
			edges[br].orientation ^= 1;
			edges[db].orientation ^= 1;
			edges[bl].orientation ^= 1;

			hold = corners[ulb];
			corners[ulb] = corners[urb];
			corners[urb] = corners[drb];
			corners[drb] = corners[dlb];
			corners[dlb] = hold;

			updateCornerOrientationZ(ulb);
			updateCornerOrientationZ(urb);
			updateCornerOrientationZ(drb);
			updateCornerOrientationZ(dlb);
			break;
		case Bp:
			hold = edges[ub];
			edges[ub] = edges[bl];
			edges[bl] = edges[db];
			edges[db] = edges[br];
			edges[br] = hold;

			edges[ub].orientation ^= 1;
			edges[br].orientation ^= 1;
			edges[db].orientation ^= 1;
			edges[bl].orientation ^= 1;

			hold = corners[ulb];
			corners[ulb] = corners[dlb];
			corners[dlb] = corners[drb];
			corners[drb] = corners[urb];
			corners[urb] = hold;

			updateCornerOrientationZ(ulb);
			updateCornerOrientationZ(urb);
			updateCornerOrientationZ(drb);
			updateCornerOrientationZ(dlb);
			break;
		case B2:
			std::swap(edges[ub], edges[db]);
			std::swap(edges[bl], edges[br]);

			std::swap(corners[ulb], corners[drb]);
			std::swap(corners[urb], corners[dlb]);
			break;
		case D:
			hold = edges[db];
			edges[db] = edges[dr];
			edges[dr] = edges[df];
			edges[df] = edges[dl];
			edges[dl] = hold;

			hold = corners[dlb];
			corners[dlb] = corners[drb];
			corners[drb] = corners[drf];
			corners[drf] = corners[dlf];
			corners[dlf] = hold;
			break;
		case Dp:
			hold = edges[dl];
			edges[dl] = edges[df];
			edges[df] = edges[dr];
			edges[dr] = edges[db];
			edges[db] = hold;

			hold = corners[dlf];
			corners[dlf] = corners[drf];
			corners[drf] = corners[drb];
			corners[drb] = corners[dlb];
			corners[dlb] = hold;
			break;
		case D2:
			std::swap(edges[db], edges[df]);
			std::swap(edges[dr], edges[dl]);

			std::swap(corners[dlb], corners[drf]);
			std::swap(corners[drb], corners[dlf]);
			break;
		};

	}
};

#endif
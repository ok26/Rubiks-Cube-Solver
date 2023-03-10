#include "cubeIndexModel.h"

#include <fstream>
#include <string>
#include <array>
#include <queue>
#include <map>

using namespace std;

int64_t ThistlewaiteID(int phase, array<Cubie, 12> edges, array<Cubie, 8> corners) {
    int64_t ID = 1;

    if (phase == 1) {
        for (int i = 0; i < 12; i++) {
            ID = ID << 1;
            ID += edges[i].orientation;
        }
        return ID;
    }
    else if (phase == 2) {
        for (int i = 0; i < 7; i++) {
			ID = ID << 2;
			ID += corners[i].orientation;
		}

		for (int i = 0; i < 12; i++) {
			ID = ID << 1;
			int index = edges[i].index;
			if (index > 3 && index < 8)
				ID++;
		}
        return ID;
    }
    else if (phase == 3) {
        std::array<Cubie, 8> cornersCopy = corners;
        if (corners[0].index == 2 && corners[2].index == 0) std::swap(cornersCopy[0], cornersCopy[2]);
        if (corners[1].index == 3 && corners[3].index == 1) std::swap(cornersCopy[1], cornersCopy[3]);
        if (corners[4].index == 6 && corners[6].index == 4) std::swap(cornersCopy[4], cornersCopy[6]);
        if (corners[5].index == 7 && corners[7].index == 5) std::swap(cornersCopy[5], cornersCopy[7]);

        for (int i = 0; i < 8; i++) {
            ID = ID << 3;
            ID += cornersCopy[i].index;
        }
        
        for (int i = 0; i < 12; i++) {
            ID = ID << 1;
            int index = edges[i].index;
            if (index == 0 || index == 2 || index == 8 || index == 10)
                ID++;
        }
        int parity = 0;
        for (int i = 0; i < 8; i++)
            for (int j = i + 1; j < 8; j++)
                parity ^= (corners[i].index < corners[j].index);
        ID = ID << 1;
        ID += parity;

        return ID;
    }
    else {
        for (int i = 0; i < 12; i++) {
            if (edges[i].index < 4)
                ID = ID << 2;
            else if (edges[i].index < 8)
                ID = ID << 3;
            else
                ID = ID << 4;
            ID += edges[i].index;
        }
        for (int i = 0; i < 8; i++) {
            ID = ID << 3;
            ID += corners[i].index;
        }
        return ID;
    }
}

int64_t KorfsID(int phase, int type, array<Cubie, 12> edges, array<Cubie, 8> corners) {
    int64_t ID = 1;
    if (phase == 1) {
        if (type == 0) {
            for (int i = 0; i < 12; i++) {
                ID = ID << 1;
                ID += edges[i].orientation;
            }
            for (int i = 0; i < 12; i++) {
                ID = ID << 1;
                int index = edges[i].index;
                if (index > 3 && index < 8)
                    ID++;
            }
            return ID;
        }
        else {
            for (int i = 0; i < 7; i++) {
                ID = ID << 2;
                ID += corners[i].orientation;
            }
            for (int i = 0; i < 12; i++) {
                ID = ID << 1;
                int index = edges[i].index;
                if (index > 3 && index < 8)
                    ID++;
            }
            return ID;
        }
    } 
    else {
        if (type == 0) {
            for (int i = 0; i < 12; i++) {
                ID = ID << 4;
                ID += edges[i].index;
            }
            return ID;
        }
        else {
            for (int i = 0; i < 8; i++) {
                ID = ID << 3;
                ID += corners[i].index;
            }
            for (int i = 4; i < 8; i++) {
                ID = ID << 4;
                ID += edges[i].index;
            }
            return ID;
        }
    }
}

bool pruneMove(int move, int lastMove) {
    if ((move == F || move == Fp || move == F2) &&
        (lastMove == F || lastMove == Fp || lastMove == F2))
        return true;
    if ((move == B || move == Bp || move == B2) &&
        (lastMove == B || lastMove == Bp || lastMove == B2))
        return true;
    if ((move == U || move == Up || move == U2) &&
        (lastMove == U || lastMove == Up || lastMove == U2))
        return true;
    if ((move == D || move == Dp || move == D2) &&
        (lastMove == D || lastMove == Dp || lastMove == D2))
        return true;
    if ((move == L || move == Lp || move == L2) &&
        (lastMove == L || lastMove == Lp || lastMove == L2))
        return true;
    if ((move == R || move == Rp || move == R2) &&
        (lastMove == R || lastMove == Rp || lastMove == R2))
        return true;
    if ((move == R || move == Rp || move == R2) &&
        (lastMove == L || lastMove == Lp || lastMove == L2))
        return true;
    if ((move == F || move == Fp || move == F2) &&
        (lastMove == B || lastMove == Bp || lastMove == B2))
        return true;
    if ((move == U || move == Up || move == U2) &&
        (lastMove == D || lastMove == Dp || lastMove == D2))
        return true;
    return false;
}

struct Node {
    CubeIndexModel state;
    int moveNr;
    int lastMove;
};

void databaseGen(int phase, int type, string file) {
    ofstream output(file);
    CubeIndexModel solvedCube = CubeIndexModel();

    int64_t solvedID = ThistlewaiteID(phase, solvedCube.edges, solvedCube.corners);
    int applicableMoves[4] = { 262143, 151551, 151524, 149796};
    //int64_t solvedID = KorfsID(phase, type, solvedCube.edges, solvedCube.corners);
    //int applicableMoves[2] = { 262143, 151524 };

    output << solvedID << " " << 0 << "\n";
    queue<Node> q;
    q.push({ solvedCube, 0, -1 });
    map<int64_t, bool> vis;
    vis[solvedID] = true;
    while (!q.empty()) {
        Node parent = q.front();
        q.pop();
        for (int move = 0; move < 18; move++) {
            if (applicableMoves[phase - 1] & (1 << move) && !pruneMove(move, parent.lastMove)) { 
                Node child = parent;
                child.state.doMove(move);
                child.moveNr++;
                child.lastMove = move;

                int64_t childID = ThistlewaiteID(phase, child.state.edges, child.state.corners);
                //int64_t childID = KorfsID(phase, type, child.state.edges, child.state.corners);

                if (!vis[childID]) {
                    vis[childID] = true;
                    q.push(child);
                    output << childID << " " << child.moveNr << "\n";
                }
            }
        }
    }
    output.close();
}

int main() {
    databaseGen(4, 0, "ThistlewaiteG4");
    return 0;
}
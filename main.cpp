#include <iostream>
#include <cstdint>
using namespace std;

using Bitboard = uint64_t;
    
    struct Position
    {
        Bitboard whitePawns;
        Bitboard whiteKnights;
        Bitboard whiteBishops;
        Bitboard whiteRooks;
        Bitboard whiteQueens;
        Bitboard whiteKing;

        Bitboard blackPawns;
        Bitboard blackKnights;
        Bitboard blackBishops;
        Bitboard blackRooks;
        Bitboard blackQueens;
        Bitboard blackKing;

        Bitboard whitePieces;
        Bitboard blackPieces;
        Bitboard occupied;
    };

    enum Square
    {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8
    };

Position createStartingPosition(){
    Position pos{};
    // White pieces
    pos.whiteKing = 1ULL << E1;
    pos.whiteQueens = 1ULL << D1;
    pos.whiteRooks = (1ULL << A1) | (1ULL << H1);
    pos.whiteKnights = (1ULL << B1) | (1ULL << G1);
    pos.whiteBishops = (1ULL << C1) | (1ULL << F1);
    pos.whitePawns = 0;
    for (int sq = A2; sq <= H2; sq++)
    {
        pos.whitePawns |= 1ULL << sq;
    }

    // Black pieces
    pos.blackKing = 1ULL << E8;
    pos.blackQueens = 1ULL << D8;
    pos.blackRooks = (1ULL << A8) | (1ULL << H8);
    pos.blackKnights = (1ULL << B8) | (1ULL << G8);
    pos.blackBishops = (1ULL << C8) | (1ULL << F8);

    pos.blackPawns = 0;
    for (int sq = A7; sq <= H7; sq++)
    {
        pos.blackPawns |= 1ULL << sq;
    }

    pos.whitePieces =
    pos.whitePawns |
    pos.whiteKnights |
    pos.whiteBishops |
    pos.whiteRooks |
    pos.whiteQueens |
    pos.whiteKing;

    pos.blackPieces =
    pos.blackPawns |
    pos.blackKnights |
    pos.blackBishops |
    pos.blackRooks |
    pos.blackQueens |
    pos.blackKing;

    pos.occupied = pos.whitePieces | pos.blackPieces;
    return pos;
}

void updateOccupancy(Position& pos){
    pos.whitePieces =
        pos.whitePawns |
        pos.whiteKnights |
        pos.whiteBishops |
        pos.whiteRooks |
        pos.whiteQueens |
        pos.whiteKing;

    pos.blackPieces =
        pos.blackPawns |
        pos.blackKnights |
        pos.blackBishops |
        pos.blackRooks |
        pos.blackQueens |
        pos.blackKing;

    pos.occupied = pos.whitePieces | pos.blackPieces;
}

void printBoard(const Position& pos){
    for(int rank = 7; rank >= 0; rank--){
        cout << rank + 1 << " ";
        for(int file = 0; file < 8; file ++){
            int sq = rank * 8 + file;
                if (pos.whitePawns & (1ULL << sq))
                    cout << "P ";
                else if (pos.whiteKnights & (1ULL << sq))
                    cout << "N ";
                else if(pos.whiteBishops & (1ULL << sq))
                    cout << "B ";
                else if(pos.whiteRooks & (1ULL << sq))
                    cout<< "R ";
                else if(pos.whiteKing & (1ULL << sq))
                    cout << "K ";
                else if(pos.whiteQueens & (1ULL << sq))
                    cout << "Q ";
                else if (pos.blackPawns & (1ULL << sq))
                    cout << "p ";
                else if (pos.blackKnights & (1ULL << sq))
                    cout << "n ";
                else if(pos.blackBishops & (1ULL << sq))
                    cout << "b ";
                else if(pos.blackRooks & (1ULL << sq))
                    cout<< "r ";
                else if(pos.blackKing & (1ULL << sq))
                    cout << "k ";
                else if(pos.blackQueens & (1ULL << sq))
                    cout << "q ";
                else
                    cout << ". ";
            }
            cout << '\n';
        }
        cout << "  a b c d e f g h\n";
}

void moveWhitePawn(Position& pos, Square from, Square to){
    pos.whitePawns &= ~(1ULL << from);
    pos.whitePawns |= (1ULL << to);
    updateOccupancy(pos);
}

int main(){
    Position pos = createStartingPosition();
    printBoard(pos);
    cout << "PRINTING AFTER MOVING A PAWN\n";
    moveWhitePawn(pos, E2, E4);
    printBoard(pos);
    return 0;
}
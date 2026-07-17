#include <iostream>
#include <cstdint>
using namespace std;


using Bitboard = uint64_t;

//masking
const Bitboard FILE_A = 0x0101010101010101ULL;
const Bitboard FILE_B = 0x0202020202020202ULL;
const Bitboard FILE_G = 0x4040404040404040ULL;
const Bitboard FILE_H = 0x8080808080808080ULL;

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

void printBitboard(Bitboard b)
{
    for(int rank = 7; rank >= 0; rank--)
    {
        cout << rank + 1 << " ";
        for(int file = 0; file < 8; file++)
        {
            int sq = rank * 8 + file;
            if(b & (1ULL << sq))
                cout << "1 ";
            else
                cout << ". ";
        }
        cout << '\n';
    }
    cout << "  a b c d e f g h\n\n";
}


void moveWhitePawn(Position& pos, Square from, Square to){
    pos.whitePawns &= ~(1ULL << from); // remove it from the previous position
    pos.whitePawns |= (1ULL << to); 
    updateOccupancy(pos);
}

// void generateKnightMoves(Position&pos){
//     Bitboard noA  = knights & ~FILE_A;
//     Bitboard noAB = knights & ~(FILE_A | FILE_B);
//     Bitboard noH  = knights & ~FILE_H;
//     Bitboard noGH = knights & ~(FILE_G | FILE_H);

//     moves |= noH  << 17;
//     moves |= noA  << 15;
//     moves |= noGH << 10;
//     moves |= noAB << 6;

//     moves |= noH  >> 15;
//     moves |= noA  >> 17;
//     moves |= noGH >> 6;
//     moves |= noAB >> 10;
// }


Bitboard knightAttacks(Bitboard knights)
{
    Bitboard moves = 0;

    Bitboard noA  = knights & ~FILE_A;
    Bitboard noAB = knights & ~(FILE_A | FILE_B);
    Bitboard noH  = knights & ~FILE_H;
    Bitboard noGH = knights & ~(FILE_G | FILE_H);


    // Move upwards
    moves |= noH  << 17;   // +2 rank +1 file
    moves |= noA  << 15;   // +2 rank -1 file
    moves |= noGH << 10;   // +1 rank +2 file
    moves |= noAB << 6;    // +1 rank -2 file


    // Move downwards
    moves |= noH  >> 15;   // -2 rank +1 file
    moves |= noA  >> 17;   // -2 rank -1 file
    moves |= noGH >> 6;    // -1 rank +2 file
    moves |= noAB >> 10;   // -1 rank -2 file


    return moves;
}

Bitboard kingAttacks(Bitboard king) {
    Bitboard moves = 0;
     
    Bitboard noA = king & ~FILE_A;
    Bitboard noH = king & ~FILE_H;

    // Straight Up and Down
    moves |= king << 8;
    moves |= king >> 8;

    // Lateral Moves (Left / Right)
    moves |= noH << 1; // Right
    moves |= noA >> 1; // Left

    moves |= noH << 9; // Up-Right
    moves |= noA << 7; // Up-Left

    moves |= noA >> 9; // Down-Left
    moves |= noH >> 7; // Down-Right

    return moves;
}

Bitboard whitePawnAttacks(Bitboard pawns){
    Bitboard moves = 0;

    Bitboard noA = pawns & ~FILE_A;
    Bitboard noH = pawns & ~FILE_H;

    moves|= pawns << 8; // forward move
    moves|= pawns << 9; // right moves
    moves|= pawns << 7; // left move 

    return moves;
}

Bitboard blackPawnAttacks(Bitboard pawns){
    Bitboard moves = 0;

    Bitboard noA = pawns & ~FILE_A;
    Bitboard noH = pawns & ~FILE_H;

    moves|= pawns >> 8; // forward move
    moves|= pawns >> 9; // right moves
    moves|= pawns >> 7; // left move 

    return moves;
}

int main(){
    Position pos = createStartingPosition();
    // printBoard(pos);
    // cout << "PRINTING AFTER MOVING A PAWN\n";
    // moveWhitePawn(pos, E2, E4);
    // printBoard(pos);
    // White knights starting squares:
    // B1 = 1
    // G1 = 6

    // Bitboard whiteKnights = (1ULL << 1) |(1ULL << 6);
    // cout << "Knight attacks:\n";
    // Bitboard attacks = knightAttacks(whiteKnights);
    // printBitboard(attacks);

    // Bitboard whiteKing = (1ULL << 3) |(1ULL << 6);
    // cout << "King attacks:\n";
    // Bitboard attacks = kingAttacks(whiteKing);
    // printBitboard(attacks);

    // Bitboard whitePawns = (1ULL << 11);
    // cout<< "White Pawn attacks\n";
    // Bitboard attacks = whitePawnAttacks(whitePawns);
    // printBitboard(attacks);

    Bitboard blackPawns = (1ULL << 34);
    cout<< "Black Pawn attacks\n";
    Bitboard attacks = blackPawnAttacks(blackPawns);
    printBitboard(attacks);

    

    return 0;
}
#include <iostream>
#include <cstdint>
#include <vector>

using namespace std;

using Bitboard = uint64_t;

//masking
const Bitboard FILE_A = 0x0101010101010101ULL;
const Bitboard FILE_B = 0x0202020202020202ULL;
const Bitboard FILE_G = 0x4040404040404040ULL;
const Bitboard FILE_H = 0x8080808080808080ULL;

enum Color { WHITE, BLACK };

enum PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

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

// Forward declarations for attack generators
Bitboard knightAttacks(Bitboard knights);
Bitboard kingAttacks(Bitboard king);
Bitboard whitePawnAttacks(Bitboard pawns);
Bitboard blackPawnAttacks(Bitboard pawns);
Bitboard bishopAttacks(Square sq, Bitboard occupied);
Bitboard rookAttacks(Square sq, Bitboard occupied);
Bitboard queenAttacks(Square sq, Bitboard occupied);

struct Move
{
    Square from;
    Square to;
    int piece;
    int capturedPiece; 
};

// Helper functions
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

void printBitboard(Bitboard b){
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

// Returns the index (0-63) of the least significant 1-bit, then clears it.
inline int popLeastSignificantBit(Bitboard& bb) {
    if (bb == 0) return -1;
    int index = __builtin_ctzll(bb); 
    bb &= bb - 1; // Clears the lowest bit
    return index;
}

int getPieceAtSquare(const Position& pos, Square sq, Color side) {
    Bitboard mask = 1ULL << sq;
    if (side == WHITE) {
        if (pos.whitePawns & mask)   return PAWN;
        if (pos.whiteKnights & mask) return KNIGHT;
        if (pos.whiteBishops & mask) return BISHOP;
        if (pos.whiteRooks & mask)   return ROOK;
        if (pos.whiteQueens & mask)  return QUEEN;
        if (pos.whiteKing & mask)    return KING;
    } else {
        if (pos.blackPawns & mask)   return PAWN;
        if (pos.blackKnights & mask) return KNIGHT;
        if (pos.blackBishops & mask) return BISHOP;
        if (pos.blackRooks & mask)   return ROOK;
        if (pos.blackQueens & mask)  return QUEEN;
        if (pos.blackKing & mask)    return KING;
    }
    return -1; // No piece found
}

vector<Move> generateMoves(const Position& pos, Color side) {
    vector<Move> moveList;
    
    Bitboard friendlyPieces = (side == WHITE) ? pos.whitePieces : pos.blackPieces;
    Bitboard enemyPieces     = (side == WHITE) ? pos.blackPieces : pos.whitePieces;
    Bitboard notFriendly    = ~friendlyPieces;
    Color enemyColor        = (side == WHITE) ? BLACK : WHITE;

    // --- 1. KNIGHTS ---
    Bitboard knights = (side == WHITE) ? pos.whiteKnights : pos.blackKnights;
    while (knights) {
        Square from = static_cast<Square>(popLeastSignificantBit(knights));
        Bitboard targets = knightAttacks(1ULL << from) & notFriendly;
        while (targets) {
            Square to = static_cast<Square>(popLeastSignificantBit(targets));
            int captured = -1;
            if (enemyPieces & (1ULL << to)) {
                captured = getPieceAtSquare(pos, to, enemyColor);
            }
            moveList.push_back({from, to, KNIGHT, captured});
        }
    }

    // --- 2. BISHOPS / ROOKS / QUEENS ---
    auto generateSliding = [&](Bitboard pieces, PieceType type, Bitboard (*attackFunc)(Square, Bitboard)) {
        while (pieces) {
            Square from = static_cast<Square>(popLeastSignificantBit(pieces));
            Bitboard targets = attackFunc(from, pos.occupied) & notFriendly;
            while (targets) {
                Square to = static_cast<Square>(popLeastSignificantBit(targets));
                int captured = -1;
                if (enemyPieces & (1ULL << to)) {
                    captured = getPieceAtSquare(pos, to, enemyColor);
                }
                moveList.push_back({from, to, type, captured});
            }
        }
    };

    generateSliding((side == WHITE) ? pos.whiteBishops : pos.blackBishops, BISHOP, bishopAttacks);
    generateSliding((side == WHITE) ? pos.whiteRooks   : pos.blackRooks,   ROOK,   rookAttacks);
    generateSliding((side == WHITE) ? pos.whiteQueens  : pos.blackQueens,  QUEEN,  queenAttacks);

    // --- 3. KING ---
    Bitboard king = (side == WHITE) ? pos.whiteKing : pos.blackKing;
    if (king) {
        Square from = static_cast<Square>(popLeastSignificantBit(king));
        Bitboard targets = kingAttacks(1ULL << from) & notFriendly;
        while (targets) {
            Square to = static_cast<Square>(popLeastSignificantBit(targets));
            int captured = -1;
            if (enemyPieces & (1ULL << to)) {
                captured = getPieceAtSquare(pos, to, enemyColor);
            }
            moveList.push_back({from, to, KING, captured});
        }
    }

    // --- 4. PAWNS ---
    Bitboard pawns = (side == WHITE) ? pos.whitePawns : pos.blackPawns;
    while (pawns) {
        Square from = static_cast<Square>(popLeastSignificantBit(pawns));
        
        if (side == WHITE) {
            // Quiet Push
            Square to = static_cast<Square>(from + 8);
            if (!(pos.occupied & (1ULL << to))) {
                moveList.push_back({from, to, PAWN, -1});
                // Double Push from rank 2
                if (from >= A2 && from <= H2 && !(pos.occupied & (1ULL << (from + 16)))) {
                    moveList.push_back({from, static_cast<Square>(from + 16), PAWN, -1});
                }
            }
            // Capture targets
            Bitboard targets = whitePawnAttacks(1ULL << from) & enemyPieces;
            while (targets) {
                Square to = static_cast<Square>(popLeastSignificantBit(targets));
                int captured = getPieceAtSquare(pos, to, BLACK);
                moveList.push_back({from, to, PAWN, captured});
            }
        } else { // Black pawns
            // Quiet Push
            Square to = static_cast<Square>(from - 8);
            if (!(pos.occupied & (1ULL << to))) {
                moveList.push_back({from, to, PAWN, -1});
                // Double Push from rank 7
                if (from >= A7 && from <= H7 && !(pos.occupied & (1ULL << (from - 16)))) {
                    moveList.push_back({from, static_cast<Square>(from - 16), PAWN, -1});
                }
            }
            // Capture targets
            Bitboard targets = blackPawnAttacks(1ULL << from) & enemyPieces;
            while (targets) {
                Square to = static_cast<Square>(popLeastSignificantBit(targets));
                int captured = getPieceAtSquare(pos, to, WHITE);
                moveList.push_back({from, to, PAWN, captured});
            }
        }
    }

    return moveList;
}

void makeMove(Position& pos, Move& move, Color side) {
    Bitboard fromBB = 1ULL << move.from;
    Bitboard toBB   = 1ULL << move.to;

    // 1. Identify and store capture data
    Color enemyColor = (side == WHITE) ? BLACK : WHITE;
    move.capturedPiece = getPieceAtSquare(pos, move.to, enemyColor);

    if (move.capturedPiece != -1) {
        if (side == WHITE) {
            if (move.capturedPiece == PAWN)   pos.blackPawns   &= ~toBB;
            if (move.capturedPiece == KNIGHT) pos.blackKnights &= ~toBB;
            if (move.capturedPiece == BISHOP) pos.blackBishops &= ~toBB;
            if (move.capturedPiece == ROOK)   pos.blackRooks   &= ~toBB;
            if (move.capturedPiece == QUEEN)  pos.blackQueens  &= ~toBB;
            if (move.capturedPiece == KING)   pos.blackKing    &= ~toBB;
        } else {
            if (move.capturedPiece == PAWN)   pos.whitePawns   &= ~toBB;
            if (move.capturedPiece == KNIGHT) pos.whiteKnights &= ~toBB;
            if (move.capturedPiece == BISHOP) pos.whiteBishops &= ~toBB;
            if (move.capturedPiece == ROOK)   pos.whiteRooks   &= ~toBB;
            if (move.capturedPiece == QUEEN)  pos.whiteQueens  &= ~toBB;
            if (move.capturedPiece == KING)   pos.whiteKing    &= ~toBB;
        }
    }

    // 2. Move your own piece
    Bitboard* pieceBB = nullptr;
    if (side == WHITE) {
        if (move.piece == PAWN)   pieceBB = &pos.whitePawns;
        if (move.piece == KNIGHT) pieceBB = &pos.whiteKnights;
        if (move.piece == BISHOP) pieceBB = &pos.whiteBishops;
        if (move.piece == ROOK)   pieceBB = &pos.whiteRooks;
        if (move.piece == QUEEN)  pieceBB = &pos.whiteQueens;
        if (move.piece == KING)   pieceBB = &pos.whiteKing;
    } else {
        if (move.piece == PAWN)   pieceBB = &pos.blackPawns;
        if (move.piece == KNIGHT) pieceBB = &pos.blackKnights;
        if (move.piece == BISHOP) pieceBB = &pos.blackBishops;
        if (move.piece == ROOK)   pieceBB = &pos.blackRooks;
        if (move.piece == QUEEN)  pieceBB = &pos.blackQueens;
        if (move.piece == KING)   pieceBB = &pos.blackKing;
    }

    if (pieceBB) {
        *pieceBB &= ~fromBB;
        *pieceBB |= toBB;
    }

    updateOccupancy(pos);
}

void undoMove(Position& pos, const Move& move, Color side) {
    Bitboard fromBB = 1ULL << move.from;
    Bitboard toBB   = 1ULL << move.to;

    // 1. Move your piece back
    Bitboard* pieceBB = nullptr;
    if (side == WHITE) {
        if (move.piece == PAWN)   pieceBB = &pos.whitePawns;
        if (move.piece == KNIGHT) pieceBB = &pos.whiteKnights;
        if (move.piece == BISHOP) pieceBB = &pos.whiteBishops;
        if (move.piece == ROOK)   pieceBB = &pos.whiteRooks;
        if (move.piece == QUEEN)  pieceBB = &pos.whiteQueens;
        if (move.piece == KING)   pieceBB = &pos.whiteKing;
    } else {
        if (move.piece == PAWN)   pieceBB = &pos.blackPawns;
        if (move.piece == KNIGHT) pieceBB = &pos.blackKnights;
        if (move.piece == BISHOP) pieceBB = &pos.blackBishops;
        if (move.piece == ROOK)   pieceBB = &pos.blackRooks;
        if (move.piece == QUEEN)  pieceBB = &pos.blackQueens;
        if (move.piece == KING)   pieceBB = &pos.blackKing;
    }

    if (pieceBB) {
        *pieceBB |= fromBB;  // Put it back on the starting square
        *pieceBB &= ~toBB;   // Clear it from the landing square
    }

    // 2. Resurrect any piece that was captured
    if (move.capturedPiece != -1) {
        if (side == WHITE) { // Opponent was Black
            if (move.capturedPiece == PAWN)   pos.blackPawns   |= toBB;
            if (move.capturedPiece == KNIGHT) pos.blackKnights |= toBB;
            if (move.capturedPiece == BISHOP) pos.blackBishops |= toBB;
            if (move.capturedPiece == ROOK)   pos.blackRooks   |= toBB;
            if (move.capturedPiece == QUEEN)  pos.blackQueens  |= toBB;
            if (move.capturedPiece == KING)   pos.blackKing    |= toBB;
        } else { // Opponent was White
            if (move.capturedPiece == PAWN)   pos.whitePawns   |= toBB;
            if (move.capturedPiece == KNIGHT) pos.whiteKnights |= toBB;
            if (move.capturedPiece == BISHOP) pos.whiteBishops |= toBB;
            if (move.capturedPiece == ROOK)   pos.whiteRooks   |= toBB;
            if (move.capturedPiece == QUEEN)  pos.whiteQueens  |= toBB;
            if (move.capturedPiece == KING)   pos.whiteKing    |= toBB;
        }
    }

    updateOccupancy(pos);
}

bool isSquareAttacked(const Position& pos, Square sq, Color attackerColor) {
    Bitboard target = 1ULL << sq;
    if (attackerColor == WHITE) {
        if (blackPawnAttacks(target) & pos.whitePawns) return true;
        if (knightAttacks(target) & pos.whiteKnights) return true;
        if (kingAttacks(target) & pos.whiteKing) return true;
        if (bishopAttacks(sq, pos.occupied) & (pos.whiteBishops | pos.whiteQueens)) return true;
        if (rookAttacks(sq, pos.occupied) & (pos.whiteRooks | pos.whiteQueens)) return true;
    } 
    else {
        if (whitePawnAttacks(target) & pos.blackPawns) return true;
        if (knightAttacks(target) & pos.blackKnights) return true;
        if (kingAttacks(target) & pos.blackKing) return true;
        if (bishopAttacks(sq, pos.occupied) & (pos.blackBishops | pos.blackQueens)) return true;
        if (rookAttacks(sq, pos.occupied) & (pos.blackRooks | pos.blackQueens)) return true;
    }
    return false;
}

// legal moves
vector<Move> getLegalMoves(Position& pos, Color side) {
    vector<Move> pseudoMoves = generateMoves(pos, side);
    vector<Move> legalMoves;
    Color enemyColor = (side == WHITE) ? BLACK : WHITE;

    for (Move& m : pseudoMoves) {
        makeMove(pos, m, side);

        Bitboard kingBB = (side == WHITE) ? pos.whiteKing : pos.blackKing;
        if (kingBB) {
            Square kingSq = static_cast<Square>(__builtin_ctzll(kingBB));
            if (!isSquareAttacked(pos, kingSq, enemyColor)) {
                legalMoves.push_back(m);
            }
        }
        undoMove(pos, m, side);
    }
    return legalMoves;
}

// Moves + attacks
void moveWhitePawn(Position& pos, Square from, Square to){
    pos.whitePawns &= ~(1ULL << from); // remove it from the previous position
    pos.whitePawns |= (1ULL << to); 
    updateOccupancy(pos);
}

Bitboard knightAttacks(Bitboard knights){
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

Bitboard whitePawnAttacks(Bitboard pawns) {
    Bitboard noA = pawns & ~FILE_A;
    Bitboard noH = pawns & ~FILE_H;

    Bitboard attacks = 0;
    attacks |= noA << 7; // Diagonal Up-Left
    attacks |= noH << 9; // Diagonal Up-Right
    return attacks;
}

Bitboard blackPawnAttacks(Bitboard pawns) {
    Bitboard noA = pawns & ~FILE_A;
    Bitboard noH = pawns & ~FILE_H;

    Bitboard attacks = 0;
    attacks |= noA >> 9; // Diagonal Down-Left
    attacks |= noH >> 7; // Diagonal Down-Right
    return attacks;
}

// Sliding pieces
Bitboard bishopAttacks(Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    
    int dr[4] = { 1, 1, -1, -1 };
    int df[4] = { 1, -1, 1, -1 };
    
    int startRank = sq / 8;
    int startFile = sq % 8;

    for (int i = 0; i < 4; i++) {
        int r = startRank + dr[i];
        int f = startFile + df[i];

        // keep sliding in this direction until hit the edge of the board
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int targetSq = r * 8 + f;
            attacks |= (1ULL << targetSq);

            // stop on blocked
            if (occupied & (1ULL << targetSq)) {
                break;
            }
            r += dr[i];
            f += df[i];
        }
    }
    return attacks;
}

Bitboard rookAttacks(Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    
    // Up, Down, Right, Left
    int dr[4] = { 1, -1, 0, 0 };
    int df[4] = { 0, 0, 1, -1 };
    
    int startRank = sq / 8;
    int startFile = sq % 8;

    for (int i = 0; i < 4; i++) {
        int r = startRank + dr[i];
        int f = startFile + df[i];

        // slide until hit the board edge
        while (r >= 0 && r < 8 && f >= 0 && f < 8) {
            int targetSq = r * 8 + f;
            attacks |= (1ULL << targetSq);

            // stop sliding if blocked.
            if (occupied & (1ULL << targetSq)) {
                break;
            }
            r += dr[i];
            f += df[i];
        }
    }
    return attacks;
}

Bitboard queenAttacks(Square sq, Bitboard occupied) {
    return rookAttacks(sq, occupied) | bishopAttacks(sq, occupied);
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

    // // 1. Test White Pawn Attacks (e.g., Pawn on D2)
    // Bitboard d2Pawn = (1ULL << D2);
    // cout << "White Pawn (D2) attacks:\n";
    // printBitboard(whitePawnAttacks(d2Pawn));

    // // 2. Test Bishop Attacks (e.g., Bishop on D4, with starting board blockers)
    // Square bishopSquare = D4;
    // cout << "Bishop on D4 attacks (considering starting position occupancy):\n";
    // Bitboard attacks = bishopAttacks(bishopSquare, pos.occupied);
    // printBitboard(attacks);

    // // 1. Test Rook Attacks from D4
    // Square rookSquare = D4;
    // cout << "Rook on D4 attacks:\n";
    // Bitboard rAttacks = rookAttacks(rookSquare, pos.occupied);
    // printBitboard(rAttacks);

    // // 2. Test Queen Attacks from D4
    // Square queenSquare = D4;
    // cout << "Queen on D4 attacks:\n";
    // Bitboard qAttacks = queenAttacks(queenSquare, pos.occupied);
    // printBitboard(qAttacks);

    cout << "--- Starting Position ---\n";
    printBoard(pos);

    // 1. Generate all initial moves for White
    vector<Move> whiteMoves = generateMoves(pos, WHITE);
    cout << "Total legal/pseudo-legal moves for White: " << whiteMoves.size() << "\n\n";

    // 2. Find and execute a specific move (E2 to E4)
    Move e2e4;
    bool found = false;
    for (const auto& m : whiteMoves) {
        if (m.from == E2 && m.to == E4) {
            e2e4 = m;
            found = true;
            break;
        }
    }

    if (found) {
        cout << "Executing E2 -> E4...\n";
        makeMove(pos, e2e4, WHITE);
        printBoard(pos);
    }
    // 3. See what Black can do next
    vector<Move> blackMoves = generateMoves(pos, BLACK);
    cout << "Total legal/pseudo-legal moves for Black: " << blackMoves.size() << "\n";

    return 0;
}
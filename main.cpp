#include <iostream>
#include <cstdint>
#include <vector>
#include <sstream>
#include <cctype>
#include <unordered_map>

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

// Explicit Move Types
enum MoveType { NORMAL, CASTLE_KING, CASTLE_QUEEN, EN_PASSANT };

// Bit-mask values for Castling Rights
const int WHITE_OO  = 1; // 0001
const int WHITE_OOO = 2; // 0010
const int BLACK_OO  = 4; // 0100
const int BLACK_OOO = 8; // 1000

enum Square
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQ_NONE 
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

    int castlingRights = 15; // 15 represents all rights active (1111 in binary)
    Square epSquare = SQ_NONE;
};


// Forward declarations for attack generators
Bitboard knightAttacks(Bitboard knights);
Bitboard kingAttacks(Bitboard king);
Bitboard whitePawnAttacks(Bitboard pawns);
Bitboard blackPawnAttacks(Bitboard pawns);
Bitboard bishopAttacks(Square sq, Bitboard occupied);
Bitboard rookAttacks(Square sq, Bitboard occupied);
Bitboard queenAttacks(Square sq, Bitboard occupied);

inline int popLeastSignificantBit(Bitboard& bb) {
    if (bb == 0) return -1;
    int index = __builtin_ctzll(bb); 
    bb &= bb - 1; // Clears the lowest bit
    return index;
}
struct Move
{
    Square from;
    Square to;
    int piece;
    int capturedPiece; 
    MoveType type = NORMAL;

    int prevCastlingRights = 15;
    Square prevEpSquare = SQ_NONE;
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

Color parseFEN(Position& pos, const string& fen) {
    // Reset the position struct completely
    pos = Position{};

    stringstream ss(fen);
    string piecePlacement, sideToMove, castleStr, epStr;
    ss >> piecePlacement >> sideToMove >> castleStr >> epStr;

    // Parse Piece Placement
    int rank = 7; // FEN starts from rank 8 (index 7) down to rank 1 (index 0)
    int file = 0;

    for (char c : piecePlacement) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (isdigit(c)) {
            file += (c - '0'); // Skip empty squares
        } else {
            Square sq = static_cast<Square>(rank * 8 + file);
            Bitboard mask = 1ULL << sq;

            switch (c) {
                // White Pieces
                case 'P': pos.whitePawns |= mask; break;
                case 'N': pos.whiteKnights |= mask; break;
                case 'B': pos.whiteBishops |= mask; break;
                case 'R': pos.whiteRooks |= mask; break;
                case 'Q': pos.whiteQueens |= mask; break;
                case 'K': pos.whiteKing |= mask; break;
                
                // Black Pieces
                case 'p': pos.blackPawns |= mask; break;
                case 'n': pos.blackKnights |= mask; break;
                case 'b': pos.blackBishops |= mask; break;
                case 'r': pos.blackRooks |= mask; break;
                case 'q': pos.blackQueens |= mask; break;
                case 'k': pos.blackKing |= mask; break;
            }
            file++;
        }
    }

    // Update all composite tracking masks (occupied, whitePieces, etc.)
    updateOccupancy(pos);

    // Parse Castling Rights
    pos.castlingRights = 0;
    if (!castleStr.empty() && castleStr != "-") {
        for (char c : castleStr) {
            if (c == 'K') pos.castlingRights |= WHITE_OO;
            if (c == 'Q') pos.castlingRights |= WHITE_OOO;
            if (c == 'k') pos.castlingRights |= BLACK_OO;
            if (c == 'q') pos.castlingRights |= BLACK_OOO;
        }
    }

    // Parse En Passant Target Square
    if (epStr == "-" || epStr.empty()) {
        pos.epSquare = SQ_NONE;
    } else {
        int f = epStr[0] - 'a';
        int r = epStr[1] - '1';
        pos.epSquare = static_cast<Square>(r * 8 + f);
    }

    // Return Side to Move safely at the bottom
    return (sideToMove == "w") ? WHITE : BLACK;
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

bool isSquareAttacked(const Position& pos, Square sq, Color attackerColor){
    Bitboard target = 1ULL << sq;
    if (attackerColor == WHITE) {
        // If a White pawn is down-left or down-right of the square, it attacks it
        Bitboard attackers = 0;
        attackers |= (target >> 7) & ~FILE_A;
        attackers |= (target >> 9) & ~FILE_H;
        if (attackers & pos.whitePawns) return true;

        if (knightAttacks(target) & pos.whiteKnights) return true;
        if (kingAttacks(target) & pos.whiteKing) return true;
        if (bishopAttacks(sq, pos.occupied) & (pos.whiteBishops | pos.whiteQueens)) return true;
        if (rookAttacks(sq, pos.occupied) & (pos.whiteRooks | pos.whiteQueens)) return true;
    } 
    else {
        // If a Black pawn is up-left or up-right of the square, it attacks it
        Bitboard attackers = 0;
        attackers |= (target << 9) & ~FILE_A;
        attackers |= (target << 7) & ~FILE_H;
        if (attackers & pos.blackPawns) return true;

        if (knightAttacks(target) & pos.blackKnights) return true;
        if (kingAttacks(target) & pos.blackKing) return true;
        if (bishopAttacks(sq, pos.occupied) & (pos.blackBishops | pos.blackQueens)) return true;
        if (rookAttacks(sq, pos.occupied) & (pos.blackRooks | pos.blackQueens)) return true;
    }
    return false;
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
            int captured = (enemyPieces & (1ULL << to)) ? getPieceAtSquare(pos, to, enemyColor) : -1;
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
                int captured = (enemyPieces & (1ULL << to)) ? getPieceAtSquare(pos, to, enemyColor) : -1;
                moveList.push_back({from, to, type, captured});
            }
        }
    };

    generateSliding((side == WHITE) ? pos.whiteBishops : pos.blackBishops, BISHOP, bishopAttacks);
    generateSliding((side == WHITE) ? pos.whiteRooks   : pos.blackRooks,   ROOK,   rookAttacks);
    generateSliding((side == WHITE) ? pos.whiteQueens  : pos.blackQueens,  QUEEN,  queenAttacks);

    // --- 3. KING & CASTLING ---
    Bitboard king = (side == WHITE) ? pos.whiteKing : pos.blackKing;
    if (king) {
        Square from = static_cast<Square>(popLeastSignificantBit(king));
        Bitboard targets = kingAttacks(1ULL << from) & notFriendly;
        while (targets) {
            Square to = static_cast<Square>(popLeastSignificantBit(targets));
            int captured = (enemyPieces & (1ULL << to)) ? getPieceAtSquare(pos, to, enemyColor) : -1;
            moveList.push_back({from, to, KING, captured});
        }

        // Castling logic (Check rights, empty squares, and square attacks)
        if (side == WHITE) {
            if (!isSquareAttacked(pos, E1, BLACK)) {
                if ((pos.castlingRights & WHITE_OO) && !(pos.occupied & ((1ULL << F1) | (1ULL << G1)))) {
                    if (!isSquareAttacked(pos, F1, BLACK) && !isSquareAttacked(pos, G1, BLACK)) {
                        moveList.push_back({E1, G1, KING, -1, CASTLE_KING});
                    }
                }
                if ((pos.castlingRights & WHITE_OOO) && !(pos.occupied & ((1ULL << D1) | (1ULL << C1) | (1ULL << B1)))) {
                    if (!isSquareAttacked(pos, D1, BLACK) && !isSquareAttacked(pos, C1, BLACK)) {
                        moveList.push_back({E1, C1, KING, -1, CASTLE_QUEEN});
                    }
                }
            }
        } else {
            if (!isSquareAttacked(pos, E8, WHITE)) {
                if ((pos.castlingRights & BLACK_OO) && !(pos.occupied & ((1ULL << F8) | (1ULL << G8)))) {
                    if (!isSquareAttacked(pos, F8, WHITE) && !isSquareAttacked(pos, G8, WHITE)) {
                        moveList.push_back({E8, G8, KING, -1, CASTLE_KING});
                    }
                }
                if ((pos.castlingRights & BLACK_OOO) && !(pos.occupied & ((1ULL << D8) | (1ULL << C8) | (1ULL << B8)))) {
                    if (!isSquareAttacked(pos, D8, WHITE) && !isSquareAttacked(pos, C8, WHITE)) {
                        moveList.push_back({E8, C8, KING, -1, CASTLE_QUEEN});
                    }
                }
            }
        }
    }

    // --- 4. PAWNS & EN PASSANT ---
    Bitboard pawns = (side == WHITE) ? pos.whitePawns : pos.blackPawns;
    while (pawns) {
        Square from = static_cast<Square>(popLeastSignificantBit(pawns));
        
        if (side == WHITE) {
            Square to = static_cast<Square>(from + 8);
            if (!(pos.occupied & (1ULL << to))) {
                moveList.push_back({from, to, PAWN, -1});
                if (from >= A2 && from <= H2 && !(pos.occupied & (1ULL << (from + 16)))) {
                    moveList.push_back({from, static_cast<Square>(from + 16), PAWN, -1});
                }
            }
            Bitboard targets = whitePawnAttacks(1ULL << from) & enemyPieces;
            while (targets) {
                Square toSq = static_cast<Square>(popLeastSignificantBit(targets));
                moveList.push_back({from, toSq, PAWN, getPieceAtSquare(pos, toSq, BLACK)});
            }
            if (pos.epSquare != SQ_NONE) {
                Bitboard epMask = 1ULL << pos.epSquare;
                if (whitePawnAttacks(1ULL << from) & epMask) {
                    moveList.push_back({from, pos.epSquare, PAWN, PAWN, EN_PASSANT});
                }
            }
        } else { 
            Square to = static_cast<Square>(from - 8);
            if (!(pos.occupied & (1ULL << to))) {
                moveList.push_back({from, to, PAWN, -1});
                if (from >= A7 && from <= H7 && !(pos.occupied & (1ULL << (from - 16)))) {
                    moveList.push_back({from, static_cast<Square>(from - 16), PAWN, -1});
                }
            }
            Bitboard targets = blackPawnAttacks(1ULL << from) & enemyPieces;
            while (targets) {
                Square toSq = static_cast<Square>(popLeastSignificantBit(targets));
                moveList.push_back({from, toSq, PAWN, getPieceAtSquare(pos, toSq, WHITE)});
            }
            if (pos.epSquare != SQ_NONE) {
                Bitboard epMask = 1ULL << pos.epSquare;
                if (blackPawnAttacks(1ULL << from) & epMask) {
                    moveList.push_back({from, pos.epSquare, PAWN, PAWN, EN_PASSANT});
                }
            }
        }
    }

    return moveList;
}

void makeMove(Position& pos, Move& move, Color side) {
    move.prevCastlingRights = pos.castlingRights;
    move.prevEpSquare = pos.epSquare;

    Bitboard fromBB = 1ULL << move.from;
    Bitboard toBB   = 1ULL << move.to;

    // Handle standard captures
    if (move.capturedPiece != -1 && move.type != EN_PASSANT) {
        if (side == WHITE) {
            if (move.capturedPiece == PAWN)   pos.blackPawns   &= ~toBB;
            if (move.capturedPiece == KNIGHT) pos.blackKnights &= ~toBB;
            if (move.capturedPiece == BISHOP) pos.blackBishops &= ~toBB;
            if (move.capturedPiece == ROOK)   pos.blackRooks   &= ~toBB;
            if (move.capturedPiece == QUEEN)  pos.blackQueens  &= ~toBB;
        } else {
            if (move.capturedPiece == PAWN)   pos.whitePawns   &= ~toBB;
            if (move.capturedPiece == KNIGHT) pos.whiteKnights &= ~toBB;
            if (move.capturedPiece == BISHOP) pos.whiteBishops &= ~toBB;
            if (move.capturedPiece == ROOK)   pos.whiteRooks   &= ~toBB;
            if (move.capturedPiece == QUEEN)  pos.whiteQueens  &= ~toBB;
        }
    }

    pos.epSquare = SQ_NONE; // Reset defaultly

    if (move.type == NORMAL) {
        Bitboard* pieceBB = nullptr;
        if (side == WHITE) {
            if (move.piece == PAWN) {
                pieceBB = &pos.whitePawns;
                if (static_cast<int>(move.to) - static_cast<int>(move.from) == 16) 
                    pos.epSquare = static_cast<Square>(move.from + 8);
            }
            if (move.piece == KNIGHT) pieceBB = &pos.whiteKnights;
            if (move.piece == BISHOP) pieceBB = &pos.whiteBishops;
            if (move.piece == ROOK)   pieceBB = &pos.whiteRooks;
            if (move.piece == QUEEN)  pieceBB = &pos.whiteQueens;
            if (move.piece == KING)   pieceBB = &pos.whiteKing;
        } else {
            if (move.piece == PAWN) {
                pieceBB = &pos.blackPawns;
                if (static_cast<int>(move.from) - static_cast<int>(move.to) == 16) 
                    pos.epSquare = static_cast<Square>(move.from - 8);
            }
            if (move.piece == KNIGHT) pieceBB = &pos.blackKnights;
            if (move.piece == BISHOP) pieceBB = &pos.blackBishops;
            if (move.piece == ROOK)   pieceBB = &pos.blackRooks;
            if (move.piece == QUEEN)  pieceBB = &pos.blackQueens;
            if (move.piece == KING)   pieceBB = &pos.blackKing;
        }
        if (pieceBB) { *pieceBB &= ~fromBB; *pieceBB |= toBB; }

    } else if (move.type == EN_PASSANT) {
        if (side == WHITE) {
            pos.whitePawns &= ~fromBB; pos.whitePawns |= toBB;
            pos.blackPawns &= ~(1ULL << (move.to - 8));
        } else {
            pos.blackPawns &= ~fromBB; pos.blackPawns |= toBB;
            pos.whitePawns &= ~(1ULL << (move.to + 8));
        }
    } else if (move.type == CASTLE_KING) {
        if (side == WHITE) {
            pos.whiteKing &= ~(1ULL << E1); pos.whiteKing |= (1ULL << G1);
            pos.whiteRooks &= ~(1ULL << H1); pos.whiteRooks |= (1ULL << F1);
        } else {
            pos.blackKing &= ~(1ULL << E8); pos.blackKing |= (1ULL << G8);
            pos.blackRooks &= ~(1ULL << H8); pos.blackRooks |= (1ULL << F8);
        }
    } else if (move.type == CASTLE_QUEEN) {
        if (side == WHITE) {
            pos.whiteKing &= ~(1ULL << E1); pos.whiteKing |= (1ULL << C1);
            pos.whiteRooks &= ~(1ULL << A1); pos.whiteRooks |= (1ULL << D1);
        } else {
            pos.blackKing &= ~(1ULL << E8); pos.blackKing |= (1ULL << C8);
            pos.blackRooks &= ~(1ULL << A8); pos.blackRooks |= (1ULL << D8);
        }
    }

    // Castling rights status revocation
    if (move.from == E1 || move.to == E1) pos.castlingRights &= ~(WHITE_OO | WHITE_OOO);
    if (move.from == A1 || move.to == A1) pos.castlingRights &= ~WHITE_OOO;
    if (move.from == H1 || move.to == H1) pos.castlingRights &= ~WHITE_OO;
    if (move.from == E8 || move.to == E8) pos.castlingRights &= ~(BLACK_OO | BLACK_OOO);
    if (move.from == A8 || move.to == A8) pos.castlingRights &= ~BLACK_OOO;
    if (move.from == H8 || move.to == H8) pos.castlingRights &= ~BLACK_OO;

    updateOccupancy(pos);
}

void undoMove(Position& pos, const Move& move, Color side) {
    Bitboard fromBB = 1ULL << move.from;
    Bitboard toBB   = 1ULL << move.to;

    if (move.type == NORMAL) {
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
        if (pieceBB) { *pieceBB |= fromBB; *pieceBB &= ~toBB; }

        if (move.capturedPiece != -1) {
            if (side == WHITE) {
                if (move.capturedPiece == PAWN)   pos.blackPawns   |= toBB;
                if (move.capturedPiece == KNIGHT) pos.blackKnights |= toBB;
                if (move.capturedPiece == BISHOP) pos.blackBishops |= toBB;
                if (move.capturedPiece == ROOK)   pos.blackRooks   |= toBB;
                if (move.capturedPiece == QUEEN)  pos.blackQueens  |= toBB;
            } else {
                if (move.capturedPiece == PAWN)   pos.whitePawns   |= toBB;
                if (move.capturedPiece == KNIGHT) pos.whiteKnights |= toBB;
                if (move.capturedPiece == BISHOP) pos.whiteBishops |= toBB;
                if (move.capturedPiece == ROOK)   pos.whiteRooks   |= toBB;
                if (move.capturedPiece == QUEEN)  pos.whiteQueens  |= toBB;
            }
        }
    } else if (move.type == EN_PASSANT) {
        if (side == WHITE) {
            pos.whitePawns |= fromBB; pos.whitePawns &= ~toBB;
            pos.blackPawns |= (1ULL << (move.to - 8));
        } else {
            pos.blackPawns |= fromBB; pos.blackPawns &= ~toBB;
            pos.whitePawns |= (1ULL << (move.to + 8));
        }
    } else if (move.type == CASTLE_KING) {
        if (side == WHITE) {
            pos.whiteKing |= (1ULL << E1); pos.whiteKing &= ~(1ULL << G1);
            pos.whiteRooks |= (1ULL << H1); pos.whiteRooks &= ~(1ULL << F1);
        } else {
            pos.blackKing |= (1ULL << E8); pos.blackKing &= ~(1ULL << G8);
            pos.blackRooks |= (1ULL << H8); pos.blackRooks &= ~(1ULL << F8);
        }
    } else if (move.type == CASTLE_QUEEN) {
        if (side == WHITE) {
            pos.whiteKing |= (1ULL << E1); pos.whiteKing &= ~(1ULL << C1);
            pos.whiteRooks |= (1ULL << A1); pos.whiteRooks &= ~(1ULL << D1);
        } else {
            pos.blackKing |= (1ULL << E8); pos.blackKing &= ~(1ULL << C8);
            pos.blackRooks |= (1ULL << A8); pos.blackRooks &= ~(1ULL << D8);
        }
    }

    pos.castlingRights = move.prevCastlingRights;
    pos.epSquare = move.prevEpSquare;
    updateOccupancy(pos);
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
            // Safely grab index using tracking bitboard
            Square kingSq = static_cast<Square>(__builtin_ctzll(kingBB));
            if (!isSquareAttacked(pos, kingSq, enemyColor)) {
                legalMoves.push_back(m);
            }
        }
        undoMove(pos, m, side);
    }
    return legalMoves;
}

long long perft(int depth, Position& pos, Color side) {
    // Base case: if we hit the bottom of the tree, this position counts as 1 leaf node
    if (depth == 0) {
        return 1ULL;
    }

    long long nodes = 0;
    vector<Move> legalMoves = getLegalMoves(pos, side);
    Color nextSide = (side == WHITE) ? BLACK : WHITE;

    for (Move& m : legalMoves) {
        makeMove(pos, m, side);
        nodes += perft(depth - 1, pos, nextSide);
        undoMove(pos, m, side);
    }
    return nodes;
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

// // Maps structural piece placement to its Opening Name and Recommended Move
// const unordered_map<string, pair<string, string>> OPENING_BOOK = {
//     {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", {"Starting Position", "e2e4"}},
//     {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR", {"King's Pawn Game (1.e4)", "e7e5"}},
//     {"rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR", {"Open Game (1.e4 e5)", "g1f3"}},
//     {"rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR", {"Queen's Pawn Game (1.d4)", "d7d5"}}
// };

// Extracts just the board array portion from any incoming FEN string
string getPiecePlacementKey(const string& fen) {
    stringstream ss(fen);
    string piecePlacement;
    ss >> piecePlacement;
    return piecePlacement;
}


//useless stuff here btw
// Checks if the current board arrangement is an established opening
// void checkStandardOpening(const string& currentFen) {
//     string key = getPiecePlacementKey(currentFen);
//     auto it = OPENING_BOOK.find(key);
    
//     cout << "\n=================================\n";
//     cout << "       OPENING BOOK DETECTOR       \n";
//     cout << "=================================\n";
//     if (it != OPENING_BOOK.end()) {
//         cout << "Detected Setup : " << it->second.first << "\n";
//         cout << "Book Move Choice: " << it->second.second << "\n";
//     } else {
//         cout << "Result: Position out of book. Engine calculation required.\n";
//     }
//     cout << "=================================\n\n";
// }


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

    // cout << "--- Starting Position ---\n";
    // printBoard(pos);

    // // 1. Generate all initial moves for White
    // vector<Move> whiteMoves = generateMoves(pos, WHITE);
    // cout << "Total legal/pseudo-legal moves for White: " << whiteMoves.size() << "\n\n";

    // // 2. Find and execute a specific move (E2 to E4)
    // Move e2e4;
    // bool found = false;
    // for (const auto& m : whiteMoves) {
    //     if (m.from == E2 && m.to == E4) {
    //         e2e4 = m;
    //         found = true;
    //         break;
    //     }
    // }

    // if (found) {
    //     cout << "Executing E2 -> E4...\n";
    //     makeMove(pos, e2e4, WHITE);
    //     printBoard(pos);
    // }
    // // 3. See what Black can do next
    // vector<Move> blackMoves = generateMoves(pos, BLACK);
    // cout << "Total legal/pseudo-legal moves for Black: " << blackMoves.size() << "\n";

    // string customFen = "4r1k1/8/8/8/8/8/8/4K3 w - - 0 1";

    // cout << "Parsing FEN...\n";
    // Color sideToMove = parseFEN(pos, customFen);
    
    // printBoard(pos);
    // cout << "Side to move: " << (sideToMove == WHITE ? "White" : "Black") << "\n\n";

    // // Test your legal move generator on this FEN
    // vector<Move> legalMoves = getLegalMoves(pos, sideToMove);
    // cout << "Total fully legal moves from this position: " << legalMoves.size() << "\n";


    // string customFen = "4r1k1/8/8/8/8/8/8/4K3 w - - 0 1";
    // Color sideToMove = parseFEN(pos, customFen);
    
    // cout << "Running Perft Tests...\n";
    // cout << "Depth 1 nodes: " << perft(1, pos, sideToMove) << " (Expected: 4)\n";
    // cout << "Depth 2 nodes: " << perft(2, pos, sideToMove) << "\n";
    // cout << "Depth 3 nodes: " << perft(3, pos, sideToMove) << "\n";

    // string customFen = "4r1k1/8/8/8/8/8/8/4K3 w - - 0 1";
    // Color sideToMove = parseFEN(pos, customFen);
    
    // cout << "Running Perft Tests...\n";
    // cout << "Depth 1 nodes: " << perft(1, pos, sideToMove) << " (Expected: 4)\n";
    // cout << "Depth 2 nodes: " << perft(2, pos, sideToMove) << " (Expected: 58)\n";
    // cout << "Depth 3 nodes: " << perft(3, pos, sideToMove) << "\n";

    // // 2. Test standard opening book tracking on normal positions
    // cout << "\nTesting Opening Book matching entries:\n";
    
    // string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // checkStandardOpening(startFen);
    
    // string afterE4Fen = "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1";
    // checkStandardOpening(afterE4Fen);


    //standard opening starting position
    string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Color sideToMove = parseFEN(pos, startFen);
    
    cout << "Running Standard Start Position Perft Tests\n";
    cout << "Depth 1 nodes: " << perft(1, pos, sideToMove) << " (Expected: 20)\n";
    cout << "Depth 2 nodes: " << perft(2, pos, sideToMove) << " (Expected: 400)\n";
    cout << "Depth 3 nodes: " << perft(3, pos, sideToMove) << " (Expected: 8902)\n";

    return 0;
}
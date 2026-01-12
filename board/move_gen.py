# --- MASKS ---
RANK_3 = 0x0000000000FF0000
RANK_4 = 0x00000000FF000000
RANK_6 = 0x0000FF0000000000
RANK_5 = 0x000000FF00000000

RANK_8 = 0xFF00000000000000
RANK_1 = 0x00000000000000FF

# file masks
FILE_A = 0x0101010101010101
FILE_B = 0x0202020202020202
FILE_G = 0x4040404040404040
FILE_H = 0x8080808080808080

# push logic for pawns 
def get_white_pawn_moves(board):
    pawns = board.white_pawns
    occ = board.get_occupied()
    
    all_pushes = (pawns << 8) & ~occ
    
    promotions = all_pushes & RANK_8
    quiet_pushes = all_pushes & ~RANK_8
    
    double_pushes = (quiet_pushes & RANK_3) << 8 & ~occ
    
    return quiet_pushes | double_pushes, promotions

def get_black_pawn_moves(board):
    pawns = board.black_pawns
    occ = board.get_occupied()
    
    all_pushes = (pawns >> 8) & ~occ
    
    promotions = all_pushes & RANK_1
    quiet_pushes = all_pushes & ~RANK_1
    
    double_pushes = (quiet_pushes & RANK_6) >> 8 & ~occ
    
    return quiet_pushes | double_pushes, promotions

# Capture lofic for pawns 
def get_white_pawn_captures(board):
    pawns = board.white_pawns
    black_pieces = board.get_all_black()
    
    captures_left = (pawns << 7) & ~FILE_H & black_pieces
    
    captures_right = (pawns << 9) & ~FILE_A & black_pieces
    
    return captures_left | captures_right

def get_black_pawn_captures(board):
    pawns = board.black_pawns
    white_pieces = board.get_all_white()
    
    captures_right = (pawns >> 7) & ~FILE_A & white_pieces
    captures_left = (pawns >> 9) & ~FILE_H & white_pieces
    
    return captures_left | captures_right

# Knight moves stuff now!    
#White knight
def get_white_knight_moves(board):
    knights = board.white_knights
    own_pieces = board.get_all_white()
    
    moves = (knights << 17) & ~FILE_A
    moves |= (knights << 15) & ~FILE_H
    moves |= (knights << 10) & ~(FILE_A | FILE_B)
    moves |= (knights << 6) & ~(FILE_G | FILE_H)
    moves |= (knights >> 17) & ~FILE_H
    moves |= (knights >> 15) & ~FILE_A
    moves |= (knights >> 10) & ~(FILE_G | FILE_H)
    moves |= (knights >> 6) & ~(FILE_A | FILE_B)

    # Filter out your own pieces
    return moves & ~own_pieces

    # Final filter: Can't land on your own pieces
    return moves & ~own_pieces

#black knight move
def get_black_knight_moves(board):
    knights = board.black_knights
    own_pieces = board.get_all_black()
    
    moves = (knights << 17) & ~FILE_A
    moves |= (knights << 15) & ~FILE_H
    moves |= (knights << 10) & ~(FILE_A | FILE_B)
    moves |= (knights << 6)  & ~(FILE_G | FILE_H)
    moves |= (knights >> 17) & ~FILE_H
    moves |= (knights >> 15) & ~FILE_A
    moves |= (knights >> 10) & ~(FILE_G | FILE_H)
    moves |= (knights >> 6)  & ~(FILE_A | FILE_B)

    # Return moves that don't land on black pieces
    return moves & ~own_pieces

    # Filter out your own pieces
    return moves & ~own_pieces

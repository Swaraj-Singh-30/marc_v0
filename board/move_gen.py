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

# Capture logic for pawns 
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

#sliding moves logic 
def get_sliding_moves(square_index, occupied, own_pieces, direction_offset):
    moves = 0
    current_square = square_index
    
    while True:
        prev_file = current_square % 8
        current_square += direction_offset
        
        if not (0 <= current_square <= 63): 
            break
        
        curr_file = current_square % 8
        if abs(direction_offset) % 8 != 0: # If move has a horizontal component
            if abs(curr_file - prev_file) != 1:
                break
        
        mask = 1 << current_square

        if mask & own_pieces:
            break

        moves |= mask
        
        if mask & occupied:
            break
            
    return moves
#rook moves
def get_white_rook_moves(board):
    all_rook_moves = 0
    rooks = board.white_rooks
    occupied = board.get_occupied()
    own_pieces = board.get_all_white()
    
    for i in range(64):
        if (rooks >> i) & 1:
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, 8)
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, -8)
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, 1)
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, -1)
            
    return all_rook_moves

def get_black_rook_moves(board):
    all_rook_moves = 0
    rooks = board.black_rooks
    occupied = board.get_occupied()
    own_pieces = board.get_all_black()  
    
    for i in range(64):
        if (rooks >> i) & 1:
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, 8)
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, -8)
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, 1)
            all_rook_moves |= get_sliding_moves(i, occupied, own_pieces, -1)
            
    return all_rook_moves

#Bishop moves

def get_white_bishop_moves(board):
    all_bishop_moves = 0
    #It is bishops and not bishop!!
    bishops = board.white_bishops 
    occupied = board.get_occupied()
    own_pieces = board.get_all_white() 

    for i in range(64):
        if (bishops >> i) & 1:
            # Diagonals: NE(+9), NW(+7), SE(-7), SW(-9)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, 9)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, 7)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, -7)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, -9)
        
    return all_bishop_moves

def get_black_bishop_moves(board):
    all_bishop_moves = 0
    bishops = board.black_bishops
    occupied = board.get_occupied()
    own_pieces = board.get_all_black() 

    for i in range(64):
        if (bishops >> i) & 1:
            # Diagonals are the same for both colors
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, 9)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, 7)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, -7)
            all_bishop_moves |= get_sliding_moves(i, occupied, own_pieces, -9)
            
    return all_bishop_moves
# White Ranks
RANK_3 = 0x0000000000FF0000
RANK_4 = 0x00000000FF000000

# Black Ranks
RANK_6 = 0x0000FF0000000000
RANK_5 = 0x000000FF00000000

def get_white_pawn_moves(board):
    """Calculates all legal single-square pawn pushes for White"""
    pawns = board.white_pawns
    occupied = board.get_occupied()
    
    # Shift pawns up by 8 (one rank), single push
    single_pushes = (pawns << 8) & ~occupied

    # --- DOUBLE PUSH ---
    double_pushes = (single_pushes & RANK_3) << 8 & ~occupied

    return single_pushes | double_pushes

def get_black_pawn_moves(board):
    pawns = board.black_pawns
    occupied = board.get_occupied()
    
    # Shift pawns up by 8 (one rank), single push
    single_pushes = (pawns >> 8) & ~occupied

    # --- DOUBLE PUSH ---
    double_pushes = (single_pushes & RANK_6) >> 8 & ~occupied
    
    return single_pushes | double_pushes
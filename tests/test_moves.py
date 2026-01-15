import sys
from pathlib import Path

# Add parent directory to path so we can import board package
sys.path.insert(0, str(Path(__file__).parent.parent))

from board.board import Board
import board.move_gen as move_gen

def create_isolated_piece_board(piece_type, position, is_white=True):
    board = Board()
    
    # Clear all pieces
    board.white_pawns = board.white_knights = board.white_bishops = 0
    board.white_rooks = board.white_queens = board.white_king = 0
    board.black_pawns = board.black_knights = board.black_bishops = 0
    board.black_rooks = board.black_queens = board.black_king = 0
    
    piece_mask = 1 << position
    
    if piece_type == 'pawn':
        board.white_pawns = piece_mask if is_white else 0
        board.black_pawns = piece_mask if not is_white else 0
    elif piece_type == 'knight':
        board.white_knights = piece_mask if is_white else 0
        board.black_knights = piece_mask if not is_white else 0
    elif piece_type == 'bishop':
        board.white_bishops = piece_mask if is_white else 0
        board.black_bishops = piece_mask if not is_white else 0
    elif piece_type == 'rook':
        board.white_rooks = piece_mask if is_white else 0
        board.black_rooks = piece_mask if not is_white else 0
    elif piece_type == 'queen':
        board.white_queens = piece_mask if is_white else 0
        board.black_queens = piece_mask if not is_white else 0
    elif piece_type == 'king':
        board.white_king = piece_mask if is_white else 0
        board.black_king = piece_mask if not is_white else 0
    
    return board

def get_moves_for_piece(board, piece_type, is_white=True):
    """Get all possible moves for a piece on the board."""
    if piece_type == 'pawn':
        if is_white:
            normal, promo = move_gen.get_white_pawn_moves(board)
            moves = normal | promo
        else:
            normal, promo = move_gen.get_black_pawn_moves(board)
            moves = normal | promo
    elif piece_type == 'knight':
        if is_white:
            moves = move_gen.get_white_knight_moves(board)
        else:
            moves = move_gen.get_black_knight_moves(board)
    elif piece_type == 'rook':
        if is_white:
            moves = move_gen.get_white_rook_moves(board)
        else:
            moves = move_gen.get_black_rook_moves(board)
    elif piece_type == 'bishop':
        if is_white:
            moves = move_gen.get_white_bishop_moves(board)
        else:
            moves = move_gen.get_black_bishop_moves(board)
    elif piece_type == 'queen':
        if is_white:
            moves = move_gen.get_white_queen_moves(board)
        else:
            moves = move_gen.get_black_queen_moves(board)
    elif piece_type == 'king':
        if is_white:
            moves = move_gen.get_white_king_moves(board)
        else:
            moves = move_gen.get_black_king_moves(board)
    else:
        moves = 0
    
    return moves

def display_board_with_moves(board, moves, piece_position, piece_type, is_white):
    """Display board with piece marked as 'X' and moves marked as '*'."""
    piece_char = piece_type[0].upper() if is_white else piece_type[0].lower()
    color = "White" if is_white else "Black"
    
    print(f"\n{'='*40}")
    print(f"{color} {piece_type.upper()} at position {piece_position}")
    print(f"Legend: X = Piece | * = Available Move | . = Empty")
    print(f"{'='*40}")
    
    print("\n  a b c d e f g h")
    print("  ---------------")
    
    for rank in reversed(range(8)):
        line = f"{rank + 1}|"
        for file in range(8):
            square_index = rank * 8 + file
            mask = 1 << square_index
            
            if square_index == piece_position:
                line += " X "  # Mark the piece
            elif (moves >> square_index) & 1:
                line += " * "  # Mark available moves
            else:
                line += " . "  # Empty square
        
        print(line)
    
    print("  ---------------\n")

def test_piece_at_position(piece_type, position, is_white=True):
    """Test and display moves for a piece at a specific position."""
    board = create_isolated_piece_board(piece_type, position, is_white)
    moves = get_moves_for_piece(board, piece_type, is_white)
    
    # Convert position to notation
    file = chr(97 + (position % 8))
    rank = (position // 8) + 1
    notation = f"{file}{rank}"
    
    color = "White" if is_white else "Black"
    print(f"\n{color} {piece_type.upper()} at {notation}:")
    
    # Count and display moves
    move_count = bin(moves).count('1')
    print(f"Available moves: {move_count}")
    
    if moves > 0:
        print("Moves to squares: ", end="")
        move_list = []
        for sq in range(64):
            if (moves >> sq) & 1:
                f = chr(97 + (sq % 8))
                r = (sq // 8) + 1
                move_list.append(f"{f}{r}")
        print(", ".join(move_list))
    
    display_board_with_moves(board, moves, position, piece_type, is_white)

def square_index_from_notation(notation):
    """Convert chess notation (e.g., 'd4') to square index (0-63)."""
    file = ord(notation[0].lower()) - ord('a')  # a-h = 0-7
    rank = int(notation[1]) - 1  # 1-8 = 0-7
    return rank * 8 + file

def main():
    print("╔════════════════════════════════════════════════════════╗")
    print("║          CHESS ENGINE - PIECE MOVE TESTING             ║")
    print("╚════════════════════════════════════════════════════════╝")
    
    # Test center position (d4 = rank 3, file 3 = index 27)
    center = square_index_from_notation('d4')
    
    # Test white pieces at center
    print("\n TESTING WHITE PIECES AT d4 (CENTER):")
    test_piece_at_position('pawn', center, is_white=True)
    test_piece_at_position('knight', center, is_white=True)
    test_piece_at_position('bishop', center, is_white=True)
    test_piece_at_position('rook', center, is_white=True)
    test_piece_at_position('queen', center, is_white=True)
    test_piece_at_position('king', center, is_white=True)
    
    # Test black pieces at center
    print("\n TESTING BLACK PIECES AT d4 (CENTER):")
    test_piece_at_position('pawn', center, is_white=False)
    test_piece_at_position('knight', center, is_white=False)
    test_piece_at_position('bishop', center, is_white=False)
    test_piece_at_position('queen', center, is_white=False)
    test_piece_at_position('king', center, is_white=False)
    
    # Test edge positions
    print("\n TESTING EDGE POSITIONS:")
    corner = square_index_from_notation('a1')
    test_piece_at_position('knight', corner, is_white=True)
    test_piece_at_position('bishop', corner, is_white=True)
    test_piece_at_position('king', corner, is_white=True)
    
    edge = square_index_from_notation('h4')
    test_piece_at_position('rook', edge, is_white=True)
    test_piece_at_position('bishop', edge, is_white=True)
    test_piece_at_position('queen', edge, is_white=True)

if __name__ == "__main__":
    main()

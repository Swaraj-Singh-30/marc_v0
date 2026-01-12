from board.board import Board
from board.move_gen import get_white_pawn_moves
from board.move_gen import get_black_pawn_moves

def main():
    my_board = Board()
    
    # Calculate where pawns CAN go
    targets = get_white_pawn_moves(my_board)
    
    print("Initial Board:")
    my_board.display()
    
    print("Possible White Pawn Destinations (Bitboard):")
    my_board.display_bitboard(targets) # This helper we wrote earlier!

    targets02 = get_black_pawn_moves(my_board)
    my_board.display_bitboard(targets02)

if __name__ == "__main__":
    main()
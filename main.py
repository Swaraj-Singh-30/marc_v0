from board.board import Board
import board.move_gen as move_gen

def main():
    board = Board()
    
    print("--- Chess Engine Debug View ---")
    board.display()
    
    test_cases = {
        "White Pawn Moves": move_gen.get_white_pawn_moves(board),
        "Black Pawn Moves": move_gen.get_black_pawn_moves(board),
        "White Knight Moves": move_gen.get_white_knight_moves(board),
        "Black Knight Moves": move_gen.get_black_knight_moves(board),
        "White Rook Moves": move_gen.get_white_rook_moves(board),
        "Black Rook Moves": move_gen.get_black_rook_moves(board),
        "White Bishop Moves": move_gen.get_white_bishop_moves(board),
        "Black Bishop Moves": move_gen.get_black_bishop_moves(board),
        "White Queen Moves": move_gen.get_white_queen_moves(board),
        "Black Queen Moves": move_gen.get_black_queen_moves(board),
        "White King Moves": move_gen.get_white_king_moves(board),
        "Black King Moves": move_gen.get_black_king_moves(board),
    }

    for label, result in test_cases.items():
        print(f"\n{label}:")
        
        if isinstance(result, tuple):
            normal, promo = result
            print("--- Normal Moves ---")
            board.display_bitboard(normal)
            print("--- Promotion Moves ---")
            board.display_bitboard(promo)
        else:
            board.display_bitboard(result)

if __name__ == "__main__":
    main()
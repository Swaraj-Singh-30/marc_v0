# main.py
from board.board import Board  # This links the two files!

def main():
    # Create an instance of the board
    my_chess_board = Board()
    
    # Test it
    print("Welcome to your Chess Engine!")
    occ = my_chess_board.get_occupied()
    print(f"Current occupied squares (Bitboard): {bin(occ)}")

if __name__ == "__main__":
    main()
# board.py

class Board:
    def __init__(self):
        # White Pieces
        self.white_pawns   = 0x000000000000FF00
        self.white_knights = 0x0000000000000042
        self.white_bishops = 0x0000000000000024
        self.white_rooks   = 0x0000000000000081
        self.white_queens  = 0x0000000000000008
        self.white_king    = 0x0000000000000010

        # Black Pieces
        self.black_pawns   = 0x00FF000000000000
        self.black_knights = 0x4200000000000000
        self.black_bishops = 0x2400000000000000
        self.black_rooks   = 0x8100000000000000
        self.black_queens  = 0x0800000000000000 
        self.black_king    = 0x1000000000000000 

    def get_occupied(self):
        return (self.white_pawns | self.white_knights | self.white_bishops | 
                self.white_rooks | self.white_queens | self.white_king |
                self.black_pawns | self.black_knights | self.black_bishops | 
                self.black_rooks | self.black_queens | self.black_king)
    def display(self):
        """Shows the full chess board with pieces (P, N, B, etc.)"""
        piece_symbols = {
            'P': self.white_pawns,   'N': self.white_knights,
            'B': self.white_bishops, 'R': self.white_rooks,
            'Q': self.white_queens,  'K': self.white_king,
            'p': self.black_pawns,   'n': self.black_knights,
            'b': self.black_bishops, 'r': self.black_rooks,
            'q': self.black_queens,  'k': self.black_king
        }
        print("\n  a b c d e f g h")
        print("  ---------------")
        for rank in reversed(range(8)):
            line = f"{rank + 1}|"
            for file in range(8):
                square_index = rank * 8 + file
                mask = 1 << square_index
                char = "." 
                for symbol, bb in piece_symbols.items():
                    if bb & mask:
                        char = symbol
                        break
                line += f"{char} "
            print(line)
        print("  ---------------")

    def display_bitboard(self, bitboard):
        """Shows a specific bitboard (like legal moves) as 1s and 0s"""
        print("\n  a b c d e f g h")
        print("  ---------------")
        for rank in reversed(range(8)):
            line = f"{rank + 1}|"
            for file in range(8):
                square = rank * 8 + file
                if (bitboard >> square) & 1:
                    line += " 1 "
                else:
                    line += " . "
            print(line)
        print("  ---------------")
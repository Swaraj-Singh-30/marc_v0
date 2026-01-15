# MARC Chess Engine - Development Roadmap

A bitboard-based chess engine implementation in Python.

## Project Structure
```
marc_v0/
├── board/              # Board representation & move generation
│   ├── board.py       # Board class (bitboards)
│   ├── move_gen.py    # Move generation logic
│   └── __init__.py
├── engine/            # AI & evaluation
│   ├── evaluation.py  # Position evaluation
│   ├── minmax.py      # Minimax algorithm
│   ├── move_ordering.py # Move ordering heuristics
│   └── __init__.py
├── interface/         # User interface
│   ├── gui.py        # GUI implementation
│   └── __init__.py
├── tests/             # Testing suite
│   └── test_moves.py # Piece move testing
├── main.py           # Main entry point
├── README.md         # This file
├── requirements.txt
└── settings.py

```

---

## Phase 1: Core Move Logic (MOSTLY COMPLETE)

### Piece Movement
- [x] White pawn moves (regular & double push)
- [x] Black pawn moves (regular & double push)
- [x] White knight moves (all 8 L-shaped moves)
- [x] Black knight moves
- [x] White rook moves (4 directions)
- [x] Black rook moves
- [x] White bishop moves (4 diagonals)
- [x] Black bishop moves
- [x] White queen moves (rook + bishop moves)
- [x] Black queen moves
- [x] White king moves (8 adjacent squares)
- [x] Black king moves

### Pawn Special Moves
- [x] Pawn promotion detection (rank 8 for white, rank 1 for black)
- [ ] Pawn promotion selection (choose Q/R/B/N)
- [x] En passant capture structure (partially)
- [ ] En passant move execution

### Capture Logic
- [ ] Pawn captures (integrate get_white_pawn_captures & get_black_pawn_captures)
- [ ] Piece captures (rook, bishop, queen, knight)
- [ ] Capture move generation

### Special Moves
- [ ] Castling (kingside & queenside for both colors)
- [ ] Castling rights tracking
- [ ] Castling move validation

### Testing
- [x] test_moves.py - Visual board testing with piece isolation
- [x] Center position testing (d4)
- [x] Edge position testing (a1, h4)

---

## Phase 2: Move Validation & Legality(NOT STARTED)

### Check Detection
- [ ] Implement is_king_in_check(board, is_white)
- [ ] Detect if white king is attacked
- [ ] Detect if black king is attacked

### Move Legality
- [ ] Filter moves that leave own king in check
- [ ] get_legal_white_moves(board) - returns only legal moves
- [ ] get_legal_black_moves(board)
- [ ] Update all piece move functions to respect legality

### Game End Conditions
- [ ] Checkmate detection (no legal moves + in check)
- [ ] Stalemate detection (no legal moves + not in check)
- [ ] Insufficient material detection (K vs K, K+B vs K, etc.)
- [ ] Threefold repetition tracking
- [ ] Fifty-move rule tracking

---

## Phase 3: Game State & Logic (NOT STARTED)

### Board State Management
- [ ] Board.white_to_move tracking
- [ ] Board.castling_rights proper initialization
- [ ] Board.en_passant_sq integration
- [ ] Move execution function (apply_move)
- [ ] Move undo function (undo_move)
- [ ] Move history tracking

### Game Loop
- [ ] Initialize game state
- [ ] Player input handling
- [ ] Move validation & execution
- [ ] Game end detection
- [ ] Result announcement (win/loss/draw)

### Game Rules
- [ ] Threefold repetition draw
- [ ] Fifty-move rule draw
- [ ] Promote pawn selection
- [ ] Capture piece removal

---

## Phase 4: AI/Evaluation Engine (NOT STARTED)

### Position Evaluation (evaluation.py)
- [ ] Material evaluation (P=1, N=3, B=3, R=5, Q=9)
- [ ] Piece position scoring
- [ ] Pawn structure evaluation
- [ ] King safety evaluation
- [ ] Piece mobility evaluation
- [ ] Control of center squares

### Move Ordering (move_ordering.py)
- [ ] Sort captures first (MVV-LVA)
- [ ] Killer move heuristics
- [ ] History heuristic
- [ ] Transposition table basics

### Minimax Algorithm (minmax.py)
- [ ] Recursive minimax implementation
- [ ] Alpha-beta pruning
- [ ] Depth-limited search
- [ ] Leaf node evaluation
- [ ] Iterative deepening

### AI Features
- [ ] Adjustable search depth
- [ ] Time-based search limits
- [ ] Opening book integration (optional)
- [ ] Endgame tables (optional)

---

## Phase 5: User Interface (NOT STARTED)

### GUI Implementation (interface/gui.py)
- [ ] Board display (visual representation)
- [ ] Piece rendering
- [ ] Move input (mouse click or notation)
- [ ] Legal move highlighting
- [ ] Move animation
- [ ] Game status display (turn, check, checkmate, etc.)
- [ ] Menu system
- [ ] New game / Load game / Save game

### Mode Selection
- [ ] Human vs. AI
- [ ] AI vs. AI
- [ ] Human vs. Human (local)(not sure about this though)

---

## Phase 6: Testing & Optimization (NOT STARTED)

### Unit Tests
- [ ] Test all move generation functions
- [ ] Test move legality
- [ ] Test check/checkmate detection
- [ ] Test special moves (castling, en passant, promotion)

### Integration Tests
- [ ] Full game scenarios
- [ ] AI decision making
- [ ] Game rule enforcement

### Performance
- [ ] Move generation speed
- [ ] Minimax search optimization
- [ ] Bitboard operation efficiency
- [ ] Profile and optimize bottlenecks

### Bug Fixes & Polish
- [ ] Edge case handling
- [ ] Input validation
- [ ] Error handling
- [ ] Code documentation

---

## Current Status

**Completed:** Phase 1 (Core Move Generation)
**In Progress:** [Capture Logic](#capture-logic) and [Special Moves](#special-moves)
**Next:** Phase 2 (Move Validation & Legality)

## Notes
- Using bitboard representation (64-bit integers for board state)
- Bitwise operations for efficient move generation
- Board initialization matches standard chess starting position

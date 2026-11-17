# Engine library

`chess-library` provides the board, chess rules, move generation, and FEN/PGN
handling. Keep the code that makes this project an engine here:

- `evaluation`: assign a score to a `chess::Board`.
- `search`: choose the best legal move from a `chess::Board`.

The engine must not reimplement board state or legal-move generation.

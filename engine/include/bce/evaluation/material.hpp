#pragma once

#include <chess.hpp>

namespace bce::evaluation {

// Value of one piece of the given type, in centipawns. Kings are worth 0
// because they are never captured.
[[nodiscard]] int pieceValue(chess::PieceType type) noexcept;

// Returns material from White's perspective, measured in centipawns.
[[nodiscard]] int materialScore(const chess::Board& board) noexcept;

// Returns the static evaluation from the side to move's perspective:
// positive means the side to move is better. Negamax search needs this form.
[[nodiscard]] int evaluate(const chess::Board& board) noexcept;

} // namespace bce::evaluation

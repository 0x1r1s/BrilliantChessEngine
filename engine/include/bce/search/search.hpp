#pragma once

#include <cstdint>

#include <chess.hpp>

namespace bce::search {

// Scores at or beyond this magnitude mean "forced mate". A mate found N plies
// from the root scores MATE_SCORE - N, so faster mates score higher.
inline constexpr int MATE_SCORE = 100'000;

struct SearchLimits {
    int depth = 5; // Full-width depth in plies; quiescence search extends it.
};

struct SearchResult {
    chess::Move bestMove{chess::Move::NO_MOVE};
    int score = 0;           // Centipawns from the side to move's perspective.
    int depth = 0;           // Depth that was searched.
    std::uint64_t nodes = 0; // Positions visited, including quiescence.
};

// Searches with negamax, alpha-beta pruning, quiescence search, and
// MVV-LVA move ordering. The board is restored before the function returns.
// NO_MOVE is returned for positions without a legal move.
[[nodiscard]] SearchResult findBestMove(chess::Board& board, const SearchLimits& limits = {});

// True when the score means a forced mate (for either side).
[[nodiscard]] constexpr bool isMateScore(const int score) noexcept {
    return score >= MATE_SCORE - 1000 || score <= -(MATE_SCORE - 1000);
}

} // namespace bce::search

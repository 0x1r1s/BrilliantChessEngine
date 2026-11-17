#include "bce/search/search.hpp"

#include <algorithm>

#include "bce/evaluation/material.hpp"

namespace bce::search {
namespace {

// Larger than any real score, including mate scores. We avoid INT_MIN/INT_MAX
// because negating INT_MIN overflows.
constexpr int INF = 1'000'000;

// State shared by every node of one search.
struct SearchContext {
    std::uint64_t nodes = 0;
};

// ---------------------------------------------------------------------------
// Move ordering (step 6)
// ---------------------------------------------------------------------------

// Higher score = searched earlier. Alpha-beta prunes the most when the best
// move comes first, so good guesses here make the search much faster.
int scoreMove(const chess::Board& board, const chess::Move move) {
    int score = 0;

    if (board.isCapture(move)) {
        // MVV-LVA: Most Valuable Victim, Least Valuable Attacker.
        // Pawn takes queen is tried before queen takes pawn. getCapturing
        // handles en passant, where the destination square is empty.
        const int victim = evaluation::pieceValue(board.getCapturing<chess::PieceType>(move));
        const int attacker = evaluation::pieceValue(board.at<chess::PieceType>(move.from()));
        score += 10'000 + 10 * victim - attacker;
    }

    if (move.typeOf() == chess::Move::PROMOTION) {
        score += 9'000 + evaluation::pieceValue(move.promotionType());
    }

    return score;
}

void orderMoves(const chess::Board& board, chess::Movelist& moves) {
    // Compute each score once and store it in the move, then sort by it.
    for (chess::Move& move : moves) {
        move.setScore(static_cast<std::int16_t>(scoreMove(board, move)));
    }

    std::sort(moves.begin(), moves.end(), [](const chess::Move& a, const chess::Move& b) {
        return a.score() > b.score();
    });
}

// ---------------------------------------------------------------------------
// Quiescence search (step 5)
// ---------------------------------------------------------------------------

// Keeps searching captures until the position is "quiet", so the engine does
// not stop right after QxP and miss the recapture.
int quiesce(chess::Board& board, int alpha, const int beta, SearchContext& ctx) {
    ++ctx.nodes;

    // "Stand pat": the side to move is not forced to capture, so the static
    // evaluation is a lower bound on what it can get here.
    const int standPat = evaluation::evaluate(board);
    if (standPat >= beta) {
        return standPat;
    }
    alpha = std::max(alpha, standPat);

    chess::Movelist captures;
    chess::movegen::legalmoves<chess::movegen::MoveGenType::CAPTURE>(captures, board);
    orderMoves(board, captures);

    for (const chess::Move move : captures) {
        board.makeMove(move);
        const int score = -quiesce(board, -beta, -alpha, ctx);
        board.unmakeMove(move);

        if (score >= beta) {
            return score;
        }
        alpha = std::max(alpha, score);
    }

    return alpha;
}

// ---------------------------------------------------------------------------
// Negamax with alpha-beta (steps 2-4)
// ---------------------------------------------------------------------------

// Returns the score of `board` from the side to move's perspective.
//   depth: plies left before switching to quiescence search
//   ply:   distance from the root (used for mate distance and draw checks)
//   alpha: score the side to move is already guaranteed elsewhere
//   beta:  score above which the opponent would avoid this position
int negamax(chess::Board& board, const int depth, const int ply, int alpha, const int beta,
            SearchContext& ctx) {
    // Draw by repetition. Inside the search one repeat is enough to call it a
    // draw: if repeating is good for us now, it will still be good later.
    if (ply > 0 && board.isRepetition(1)) {
        return 0;
    }

    if (depth <= 0) {
        return quiesce(board, alpha, beta, ctx);
    }

    ++ctx.nodes;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);

    // No legal moves: checkmate or stalemate (step 3).
    if (moves.empty()) {
        // -MATE_SCORE + ply: being mated later is less bad than sooner, and
        // from the winner's side, faster mates score higher.
        return board.inCheck() ? -MATE_SCORE + ply : 0;
    }

    // Fifty-move rule. Checked after move generation, because checkmate on
    // the 100th half-move takes priority over the draw.
    if (board.isHalfMoveDraw()) {
        return 0;
    }

    orderMoves(board, moves);

    int best = -INF;
    for (const chess::Move move : moves) {
        board.makeMove(move);
        // The opponent's best score is our worst, so negate it. The window
        // is swapped and negated for the same reason.
        const int score = -negamax(board, depth - 1, ply + 1, -beta, -alpha, ctx);
        board.unmakeMove(move);

        if (score > best) {
            best = score;
        }
        if (score > alpha) {
            alpha = score;
        }
        if (alpha >= beta) {
            break; // Beta cutoff: the opponent will never allow this line.
        }
    }

    return best;
}

} // namespace

SearchResult findBestMove(chess::Board& board, const SearchLimits& limits) {
    SearchResult result;
    SearchContext ctx;

    chess::Movelist moves;
    chess::movegen::legalmoves(moves, board);
    if (moves.empty()) {
        return result;
    }

    const int depth = std::max(1, limits.depth);
    orderMoves(board, moves);

    // The root is like negamax, but it also remembers which move was best.
    int alpha = -INF;
    const int beta = INF;

    for (const chess::Move move : moves) {
        board.makeMove(move);
        const int score = -negamax(board, depth - 1, 1, -beta, -alpha, ctx);
        board.unmakeMove(move);

        if (result.bestMove == chess::Move::NO_MOVE || score > alpha) {
            alpha = score;
            result.bestMove = move;
            result.score = score;
        }
    }

    result.depth = depth;
    result.nodes = ctx.nodes;
    return result;
}

} // namespace bce::search

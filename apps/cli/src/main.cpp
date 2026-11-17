#include <cstdlib>
#include <iostream>
#include <string>

#include "bce/evaluation/material.hpp"
#include "bce/search/search.hpp"

namespace {

void printUsage(const char* program) {
    std::cout << "Usage: " << program << " [FEN] [depth]\n"
              << "  FEN    position to analyse (default: starting position)\n"
              << "  depth  search depth in plies (default: 5)\n";
}

void printScore(const int score) {
    if (bce::search::isMateScore(score)) {
        // Convert "plies to mate" into "moves to mate" for display.
        const int plies = bce::search::MATE_SCORE - std::abs(score);
        const int moves = (plies + 1) / 2;
        std::cout << (score > 0 ? "mate in " : "mated in ") << moves;
    } else {
        std::cout << score << " centipawns";
    }
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        printUsage(argv[0]);
        return 0;
    }

    chess::Board board;
    if (argc > 1 && !board.setFen(argv[1])) {
        std::cerr << "Invalid FEN: " << argv[1] << '\n';
        return 1;
    }

    bce::search::SearchLimits limits;
    if (argc > 2) {
        limits.depth = std::atoi(argv[2]);
        if (limits.depth < 1) {
            std::cerr << "Depth must be a positive number.\n";
            return 1;
        }
    }

    std::cout << "Brilliant Chess Engine\n\n";
    std::cout << board;
    std::cout << "Material evaluation (White's perspective): "
              << bce::evaluation::materialScore(board) << " centipawns\n";

    const bce::search::SearchResult result = bce::search::findBestMove(board, limits);

    if (result.bestMove == chess::Move::NO_MOVE) {
        std::cout << (board.inCheck() ? "Checkmate" : "Stalemate") << ": no legal moves.\n";
        return 0;
    }

    std::cout << "Best move (depth " << result.depth << "): "
              << chess::uci::moveToUci(result.bestMove) << '\n';
    std::cout << "Score (side to move): ";
    printScore(result.score);
    std::cout << "\nNodes searched: " << result.nodes << '\n';

    return 0;
}

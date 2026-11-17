#include "bce/evaluation/material.hpp"

namespace bce::evaluation {

int pieceValue(const chess::PieceType type) noexcept {
    if (type == chess::PieceType::PAWN) {
        return 100;
    }
    if (type == chess::PieceType::KNIGHT || type == chess::PieceType::BISHOP) {
        return 300;
    }
    if (type == chess::PieceType::ROOK) {
        return 500;
    }
    if (type == chess::PieceType::QUEEN) {
        return 900;
    }

    return 0;
}

int materialScore(const chess::Board& board) noexcept {
    int score = 0;

    for (int squareIndex = 0; squareIndex < chess::Square::max(); ++squareIndex) {
        const chess::Piece piece = board.at(chess::Square(squareIndex));
        const int value = pieceValue(piece.type());

        if (piece.color() == chess::Color::WHITE) {
            score += value;
        } else if (piece.color() == chess::Color::BLACK) {
            score -= value;
        }
    }

    return score;
}

int evaluate(const chess::Board& board) noexcept {
    const int whiteScore = materialScore(board);
    return board.sideToMove() == chess::Color::WHITE ? whiteScore : -whiteScore;
}

} // namespace bce::evaluation

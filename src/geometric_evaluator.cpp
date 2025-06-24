#include "geometric_evaluator.h"

Multivector2D GeometricEvaluator::calculate_piece_influence(PieceType piece, Square square, const Board& board) {
    switch (piece) {
        case PieceType::PAWN:
            return calculate_pawn_influence(square, board);
        case PieceType::KNIGHT:
            return calculate_knight_influence(square, board);
        case PieceType::BISHOP:
            return calculate_bishop_influence(square, board);
        case PieceType::ROOK:
            return calculate_rook_influence(square, board);
        case PieceType::QUEEN:
            return calculate_queen_influence(square, board);
        case PieceType::KING:
            return calculate_king_influence(square, board);
        default:
            return Multivector2D();
    }
}

Multivector2D GeometricEvaluator::calculate_pawn_influence(Square square, const Board& board) {
    Vector2D coords = square_to_coords(square);
    bool is_white = board.side_to_move;
    
    Vector2D forward_move, left_capture, right_capture;
    
    if (is_white) {
        forward_move = Vector2D(0.0f, 1.0f);
        left_capture = Vector2D(-1.0f, 1.0f);
        right_capture = Vector2D(1.0f, 1.0f);
    } else {
        forward_move = Vector2D(0.0f, -1.0f);
        left_capture = Vector2D(-1.0f, -1.0f);
        right_capture = Vector2D(1.0f, -1.0f);
    }
    
    Multivector2D forward_mv(forward_move);
    Multivector2D left_mv(left_capture);
    Multivector2D right_mv(right_capture);
    
    return forward_mv + left_mv + right_mv;
}

Multivector2D GeometricEvaluator::calculate_knight_influence(Square square, const Board& board) {
    const Vector2D knight_moves[8] = {
        Vector2D(1.0f, 2.0f), Vector2D(2.0f, 1.0f),
        Vector2D(2.0f, -1.0f), Vector2D(1.0f, -2.0f),
        Vector2D(-1.0f, -2.0f), Vector2D(-2.0f, -1.0f),
        Vector2D(-2.0f, 1.0f), Vector2D(-1.0f, 2.0f)
    };
    
    Multivector2D total_influence;
    for (int i = 0; i < 8; i++) {
        Multivector2D move_mv(knight_moves[i]);
        total_influence = total_influence + move_mv;
    }
    
    return total_influence;
}

Multivector2D GeometricEvaluator::calculate_bishop_influence(Square square, const Board& board) {
    uint64_t attack_bitboard = MagicBitboards::get_bishop_attacks(square, board.all_pieces);
    int attack_count = popcount(attack_bitboard);
    
    float magnitude = static_cast<float>(attack_count) / 14.0f;
    
    Bivector2D diagonal1(magnitude * 0.5f);
    Bivector2D diagonal2(magnitude * 0.5f);
    
    Multivector2D biv1(diagonal1);
    Multivector2D biv2(diagonal2);
    
    return biv1 + biv2;
}

Multivector2D GeometricEvaluator::calculate_rook_influence(Square square, const Board& board) {
    uint64_t attack_bitboard = MagicBitboards::get_rook_attacks(square, board.all_pieces);
    int attack_count = popcount(attack_bitboard);
    
    float magnitude = static_cast<float>(attack_count) / 14.0f;
    
    Bivector2D horizontal(magnitude * 0.5f);
    Bivector2D vertical(magnitude * 0.5f);
    
    Multivector2D biv1(horizontal);
    Multivector2D biv2(vertical);
    
    return biv1 + biv2;
}

Multivector2D GeometricEvaluator::calculate_queen_influence(Square square, const Board& board) {
    Multivector2D bishop_part = calculate_bishop_influence(square, board);
    Multivector2D rook_part = calculate_rook_influence(square, board);
    
    return bishop_part + rook_part;
}

Multivector2D GeometricEvaluator::calculate_king_influence(Square square, const Board& board) {
    const Vector2D king_moves[8] = {
        Vector2D(-1.0f, -1.0f), Vector2D(-1.0f, 0.0f), Vector2D(-1.0f, 1.0f),
        Vector2D(0.0f, -1.0f), Vector2D(0.0f, 1.0f),
        Vector2D(1.0f, -1.0f), Vector2D(1.0f, 0.0f), Vector2D(1.0f, 1.0f)
    };
    
    Multivector2D total_influence;
    for (int i = 0; i < 8; i++) {
        Multivector2D move_mv(king_moves[i]);
        total_influence = total_influence + move_mv;
    }
    
    return total_influence;
}

int GeometricEvaluator::popcount(uint64_t bitboard) {
    return __builtin_popcountll(bitboard);
}

Vector2D GeometricEvaluator::square_to_coords(Square square) {
    int file = square % 8;
    int rank = square / 8;
    return Vector2D(static_cast<float>(file), static_cast<float>(rank));
}

PieceType GeometricEvaluator::piece_to_type(Piece piece) {
    switch (piece) {
        case WP: case BP: return PieceType::PAWN;
        case WN: case BN: return PieceType::KNIGHT;
        case WB: case BB: return PieceType::BISHOP;
        case WR: case BR: return PieceType::ROOK;
        case WQ: case BQ: return PieceType::QUEEN;
        case WK: case BK: return PieceType::KING;
        default: return PieceType::PAWN;
    }
} 
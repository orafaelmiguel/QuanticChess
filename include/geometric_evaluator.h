#ifndef GEOMETRIC_EVALUATOR_H
#define GEOMETRIC_EVALUATOR_H

#include "geometric_algebra.h"
#include "bitboard.h"
#include "magic_bitboards.h"

enum class PieceType {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

class GeometricEvaluator {
public:
    static Multivector2D calculate_piece_influence(PieceType piece, Square square, const Board& board);
    
private:
    static Multivector2D calculate_pawn_influence(Square square, const Board& board);
    static Multivector2D calculate_knight_influence(Square square, const Board& board);
    static Multivector2D calculate_bishop_influence(Square square, const Board& board);
    static Multivector2D calculate_rook_influence(Square square, const Board& board);
    static Multivector2D calculate_queen_influence(Square square, const Board& board);
    static Multivector2D calculate_king_influence(Square square, const Board& board);
    
    static int popcount(uint64_t bitboard);
    static Vector2D square_to_coords(Square square);
    static PieceType piece_to_type(Piece piece);
};

#endif // GEOMETRIC_EVALUATOR_H 
#include <iostream>
#include <string>
#include <iomanip>
#include "GeometricState.h"
#include "bitboard.h"

void print_bitboard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (bitboard & (1ULL << square)) {
                std::cout << "1 ";
            } else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl << std::endl;
}

std::string square_to_string(Square sq) {
    int file = sq % 8;
    int rank = sq / 8;
    return std::string(1, 'a' + file) + std::string(1, '1' + rank);
}

std::string move_type_to_string(MoveType type) {
    switch (type) {
        case NORMAL: return "Normal";
        case CAPTURE: return "Capture";
        case EN_PASSANT: return "En Passant";
        case PROMOTION: return "Promotion";
        default: return "Unknown";
    }
}

std::string piece_to_string(Piece piece) {
    switch (piece) {
        case WQ: case BQ: return "Queen";
        case WR: case BR: return "Rook";
        case WB: case BB: return "Bishop";
        case WN: case BN: return "Knight";
        default: return "";
    }
}

void print_moves(const MoveList& moves) {
    std::cout << "Generated " << moves.size() << " pawn moves:" << std::endl;
    for (const auto& move : moves) {
        std::cout << square_to_string(move.from) << " -> " 
                  << square_to_string(move.to) << " (" 
                  << move_type_to_string(move.type);
        if (move.type == PROMOTION) {
            std::cout << " to " << piece_to_string(move.promotion_piece);
        }
        std::cout << ")" << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Quantum Chess Pawn Move Generation ===" << std::endl << std::endl;
    
    Board starting_board;
    MoveList moves;
    
    std::cout << "Testing starting position (White to move):" << std::endl;
    starting_board.generate_pawn_moves(moves);
    print_moves(moves);
    
    moves.clear();
    Board en_passant_board("rnbqkbnr/pp1ppppp/8/2pP4/8/8/PPP1PPPP/RNBQKBNR w KQkq c6 0 3");
    std::cout << "Testing en passant position:" << std::endl;
    std::cout << "En passant square: " << en_passant_board.en_passant_square << std::endl;
    en_passant_board.generate_pawn_moves(moves);
    print_moves(moves);
    
    moves.clear();
    Board promotion_board("rnbqkbnr/ppppppPp/8/8/8/8/PPPPPPP1/RNBQKBNR w KQkq - 0 1");
    std::cout << "Testing promotion position (White pawn on g7):" << std::endl;
    promotion_board.generate_pawn_moves(moves);
    print_moves(moves);
    
    moves.clear();
    Board black_moves_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    std::cout << "Testing starting position (Black to move):" << std::endl;
    black_moves_board.generate_pawn_moves(moves);
    print_moves(moves);

    return 0;
} 
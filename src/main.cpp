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

int main() {
    std::cout << "=== Quantum Chess Bitboard System ===" << std::endl << std::endl;
    
    Board board;
    
    std::cout << "Starting position FEN loaded:" << std::endl;
    std::cout << "Side to move: " << (board.side_to_move ? "White" : "Black") << std::endl;
    std::cout << "Castling rights: " << board.castling_rights << std::endl;
    std::cout << "En passant square: " << board.en_passant_square << std::endl << std::endl;
    
    std::cout << "White pieces bitboard:" << std::endl;
    print_bitboard(board.white_pieces);
    
    std::cout << "Black pieces bitboard:" << std::endl;
    print_bitboard(board.black_pieces);
    
    std::cout << "White pawns bitboard:" << std::endl;
    print_bitboard(board.bitboards[WP]);
    
    Board custom_board("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2");
    std::cout << "Custom position (Sicilian Defense) loaded:" << std::endl;
    std::cout << "En passant square: " << custom_board.en_passant_square << std::endl;
    std::cout << "All pieces:" << std::endl;
    print_bitboard(custom_board.all_pieces);
    
    GeometricState white_pawn_e4(ChessSquare::E4);
    std::cout << "Geometric State - White pawn at e4:" << std::endl;
    std::cout << "Position: (" << white_pawn_e4.position.x() << ", " 
              << white_pawn_e4.position.y() << ", " 
              << white_pawn_e4.position.z() << ")" << std::endl;
    
    std::cout << "Forward direction: (" << white_pawn_e4.forward_direction.x() << ", " 
              << white_pawn_e4.forward_direction.y() << ", " 
              << white_pawn_e4.forward_direction.z() << ")" << std::endl;
    
    kln::direction move_to_e5 = white_pawn_e4.get_movement_vector(ChessSquare::E5);
    std::cout << "Movement vector to e5: (" << move_to_e5.x() << ", " 
              << move_to_e5.y() << ", " << move_to_e5.z() << ")" << std::endl;

    return 0;
} 
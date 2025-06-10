#pragma once

#include <cstdint>
#include <string>

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

class Board {
public:
    uint64_t bitboards[12];
    uint64_t white_pieces;
    uint64_t black_pieces;
    uint64_t all_pieces;
    bool side_to_move;
    int en_passant_square;
    int castling_rights;
    
    Board();
    Board(const std::string& fen_string);
    
    void load_fen(const std::string& fen_string);
    void clear_board();
    void update_occupancy();
    Square string_to_square(const std::string& square_str);
    
private:
    char piece_to_char(Piece piece);
    Piece char_to_piece(char c);
}; 
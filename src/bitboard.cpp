#include "bitboard.h"
#include <sstream>
#include <iostream>

Board::Board() {
    clear_board();
    load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board::Board(const std::string& fen_string) {
    clear_board();
    load_fen(fen_string);
}

void Board::clear_board() {
    for (int i = 0; i < 12; i++) {
        bitboards[i] = 0ULL;
    }
    white_pieces = 0ULL;
    black_pieces = 0ULL;
    all_pieces = 0ULL;
    side_to_move = true;
    en_passant_square = -1;
    castling_rights = 0;
}

void Board::load_fen(const std::string& fen_string) {
    clear_board();
    
    std::istringstream fen_stream(fen_string);
    std::string board_position, active_color, castling, en_passant, halfmove, fullmove;
    
    fen_stream >> board_position >> active_color >> castling >> en_passant >> halfmove >> fullmove;
    
    int rank = 7;
    int file = 0;
    
    for (char c : board_position) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += (c - '0');
        } else {
            Square square = static_cast<Square>(rank * 8 + file);
            uint64_t square_bit = 1ULL << square;
            
            Piece piece = char_to_piece(c);
            bitboards[piece] |= square_bit;
            
            file++;
        }
    }
    
    side_to_move = (active_color == "w");
    
    castling_rights = 0;
    for (char c : castling) {
        switch (c) {
            case 'K': castling_rights |= 1; break;
            case 'Q': castling_rights |= 2; break;
            case 'k': castling_rights |= 4; break;
            case 'q': castling_rights |= 8; break;
        }
    }
    
    if (en_passant != "-") {
        en_passant_square = static_cast<int>(string_to_square(en_passant));
    } else {
        en_passant_square = -1;
    }
    
    update_occupancy();
}

void Board::update_occupancy() {
    white_pieces = 0ULL;
    black_pieces = 0ULL;
    
    for (int i = WP; i <= WK; i++) {
        white_pieces |= bitboards[i];
    }
    
    for (int i = BP; i <= BK; i++) {
        black_pieces |= bitboards[i];
    }
    
    all_pieces = white_pieces | black_pieces;
}

void Board::generate_pawn_moves(MoveList& move_list) {
    if (side_to_move) {
        uint64_t pawns = bitboards[WP];
        uint64_t empty = ~all_pieces;
        uint64_t enemy_pieces = black_pieces;
        
        uint64_t single_pushes = (pawns << 8) & empty;
        uint64_t double_pushes = ((single_pushes & 0xFF0000ULL) << 8) & empty;
        
        uint64_t left_captures = ((pawns & 0xFEFEFEFEFEFEFEFEULL) << 7) & enemy_pieces;
        uint64_t right_captures = ((pawns & 0x7F7F7F7F7F7F7F7FULL) << 9) & enemy_pieces;
        
        while (single_pushes) {
            int to = __builtin_ctzll(single_pushes);
            int from = to - 8;
            if (to >= 56) {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WQ));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WR));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WB));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WN));
            } else {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), NORMAL));
            }
            single_pushes &= single_pushes - 1;
        }
        
        while (double_pushes) {
            int to = __builtin_ctzll(double_pushes);
            int from = to - 16;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), NORMAL));
            double_pushes &= double_pushes - 1;
        }
        
        while (left_captures) {
            int to = __builtin_ctzll(left_captures);
            int from = to - 7;
            if (to >= 56) {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WQ));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WR));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WB));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WN));
            } else {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), CAPTURE));
            }
            left_captures &= left_captures - 1;
        }
        
        while (right_captures) {
            int to = __builtin_ctzll(right_captures);
            int from = to - 9;
            if (to >= 56) {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WQ));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WR));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WB));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, WN));
            } else {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), CAPTURE));
            }
            right_captures &= right_captures - 1;
        }
        
        if (en_passant_square != -1) {
            uint64_t ep_pawns = pawns & ((1ULL << (en_passant_square - 7)) | (1ULL << (en_passant_square - 9)));
            while (ep_pawns) {
                int from = __builtin_ctzll(ep_pawns);
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(en_passant_square), EN_PASSANT));
                ep_pawns &= ep_pawns - 1;
            }
        }
    } else {
        uint64_t pawns = bitboards[BP];
        uint64_t empty = ~all_pieces;
        uint64_t enemy_pieces = white_pieces;
        
        uint64_t single_pushes = (pawns >> 8) & empty;
        uint64_t double_pushes = ((single_pushes & 0xFF000000000000ULL) >> 8) & empty;
        
        uint64_t left_captures = ((pawns & 0x7F7F7F7F7F7F7F7FULL) >> 7) & enemy_pieces;
        uint64_t right_captures = ((pawns & 0xFEFEFEFEFEFEFEFEULL) >> 9) & enemy_pieces;
        
        while (single_pushes) {
            int to = __builtin_ctzll(single_pushes);
            int from = to + 8;
            if (to <= 7) {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BQ));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BR));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BB));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BN));
            } else {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), NORMAL));
            }
            single_pushes &= single_pushes - 1;
        }
        
        while (double_pushes) {
            int to = __builtin_ctzll(double_pushes);
            int from = to + 16;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), NORMAL));
            double_pushes &= double_pushes - 1;
        }
        
        while (left_captures) {
            int to = __builtin_ctzll(left_captures);
            int from = to + 7;
            if (to <= 7) {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BQ));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BR));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BB));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BN));
            } else {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), CAPTURE));
            }
            left_captures &= left_captures - 1;
        }
        
        while (right_captures) {
            int to = __builtin_ctzll(right_captures);
            int from = to + 9;
            if (to <= 7) {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BQ));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BR));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BB));
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), PROMOTION, BN));
            } else {
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), CAPTURE));
            }
            right_captures &= right_captures - 1;
        }
        
        if (en_passant_square != -1) {
            uint64_t ep_pawns = pawns & ((1ULL << (en_passant_square + 7)) | (1ULL << (en_passant_square + 9)));
            while (ep_pawns) {
                int from = __builtin_ctzll(ep_pawns);
                move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(en_passant_square), EN_PASSANT));
                ep_pawns &= ep_pawns - 1;
            }
        }
    }
}

Square Board::string_to_square(const std::string& square_str) {
    if (square_str.length() != 2) return A1;
    
    int file = square_str[0] - 'a';
    int rank = square_str[1] - '1';
    
    return static_cast<Square>(rank * 8 + file);
}

char Board::piece_to_char(Piece piece) {
    switch (piece) {
        case WP: return 'P';
        case WN: return 'N';
        case WB: return 'B';
        case WR: return 'R';
        case WQ: return 'Q';
        case WK: return 'K';
        case BP: return 'p';
        case BN: return 'n';
        case BB: return 'b';
        case BR: return 'r';
        case BQ: return 'q';
        case BK: return 'k';
        default: return '.';
    }
}

Piece Board::char_to_piece(char c) {
    switch (c) {
        case 'P': return WP;
        case 'N': return WN;
        case 'B': return WB;
        case 'R': return WR;
        case 'Q': return WQ;
        case 'K': return WK;
        case 'p': return BP;
        case 'n': return BN;
        case 'b': return BB;
        case 'r': return BR;
        case 'q': return BQ;
        case 'k': return BK;
        default: return WP;
    }
} 
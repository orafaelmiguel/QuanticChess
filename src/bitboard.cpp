#include "bitboard.h"
#include "magic_bitboards.h"
#include <sstream>
#include <iostream>

uint64_t Board::knight_attacks[64] = {0};
uint64_t Board::king_attacks[64] = {0};

const int knight_offsets[8][2] = {
    {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
    {1, -2}, {1, 2}, {2, -1}, {2, 1}
};

const int king_offsets[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1}, {0, 1},
    {1, -1}, {1, 0}, {1, 1}
};

void Board::init_attack_tables() {
    for (int square = 0; square < 64; square++) {
        int rank = square / 8;
        int file = square % 8;
        
        for (const auto& offset : knight_offsets) {
            int new_rank = rank + offset[0];
            int new_file = file + offset[1];
            
            if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
                int target_square = new_rank * 8 + new_file;
                knight_attacks[square] |= 1ULL << target_square;
            }
        }
    }
    
    for (int square = 0; square < 64; square++) {
        int rank = square / 8;
        int file = square % 8;
        
        for (const auto& offset : king_offsets) {
            int new_rank = rank + offset[0];
            int new_file = file + offset[1];
            
            if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
                int target_square = new_rank * 8 + new_file;
                king_attacks[square] |= 1ULL << target_square;
            }
        }
    }
}

Board::Board() {
    clear_board();
    load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    init_attack_tables();
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

void Board::generate_knight_moves(MoveList& move_list) {
    uint64_t knights = side_to_move ? bitboards[WN] : bitboards[BN];
    uint64_t friendly_pieces = side_to_move ? white_pieces : black_pieces;
    uint64_t enemy_pieces = side_to_move ? black_pieces : white_pieces;
    
    while (knights) {
        int from = __builtin_ctzll(knights);
        uint64_t attacks = knight_attacks[from] & ~friendly_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            MoveType type = (enemy_pieces & (1ULL << to)) ? CAPTURE : NORMAL;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), type));
            attacks &= attacks - 1;
        }
        
        knights &= knights - 1;
    }
}

bool Board::is_square_attacked(Square square, bool by_white) const {
    uint64_t square_bit = 1ULL << square;
    uint64_t attackers = by_white ? white_pieces : black_pieces;
    
    if (by_white) {
        if ((bitboards[WP] & ((square_bit >> 7) & 0xFEFEFEFEFEFEFEFEULL)) ||
            (bitboards[WP] & ((square_bit >> 9) & 0x7F7F7F7F7F7F7F7FULL))) {
            return true;
        }
    } else {
        if ((bitboards[BP] & ((square_bit << 7) & 0x7F7F7F7F7F7F7F7FULL)) ||
            (bitboards[BP] & ((square_bit << 9) & 0xFEFEFEFEFEFEFEFEULL))) {
            return true;
        }
    }
    
    if (knight_attacks[square] & (by_white ? bitboards[WN] : bitboards[BN])) {
        return true;
    }
    
    if (king_attacks[square] & (by_white ? bitboards[WK] : bitboards[BK])) {
        return true;
    }
    
    return false;
}

void Board::generate_king_moves(MoveList& move_list) {
    uint64_t king = side_to_move ? bitboards[WK] : bitboards[BK];
    uint64_t friendly_pieces = side_to_move ? white_pieces : black_pieces;
    uint64_t enemy_pieces = side_to_move ? black_pieces : white_pieces;
    
    if (king) {
        int from = __builtin_ctzll(king);
        uint64_t attacks = king_attacks[from] & ~friendly_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            MoveType type = (enemy_pieces & (1ULL << to)) ? CAPTURE : NORMAL;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), type));
            attacks &= attacks - 1;
        }
        
        if (side_to_move) {
            if ((castling_rights & 1) && // White king-side castling right
                !(all_pieces & 0x60ULL) && // Check if squares between king and rook are empty
                !is_square_attacked(F1, false) && // Check if F1 is not under attack
                !is_square_attacked(G1, false)) { // Check if G1 is not under attack
                move_list.push_back(Move(E1, G1, CASTLE_KING));
            }
            
            if ((castling_rights & 2) && // White queen-side castling right
                !(all_pieces & 0x0EULL) && // Check if squares between king and rook are empty
                !is_square_attacked(D1, false) && // Check if D1 is not under attack
                !is_square_attacked(C1, false)) { // Check if C1 is not under attack
                move_list.push_back(Move(E1, C1, CASTLE_QUEEN));
            }
        } else {
            if ((castling_rights & 4) && // Black king-side castling right
                !(all_pieces & 0x6000000000000000ULL) && // Check if squares between king and rook are empty
                !is_square_attacked(F8, true) && // Check if F8 is not under attack
                !is_square_attacked(G8, true)) { // Check if G8 is not under attack
                move_list.push_back(Move(E8, G8, CASTLE_KING));
            }
            
            if ((castling_rights & 8) && // Black queen-side castling right
                !(all_pieces & 0x0E00000000000000ULL) && // Check if squares between king and rook are empty
                !is_square_attacked(D8, true) && // Check if D8 is not under attack
                !is_square_attacked(C8, true)) { // Check if C8 is not under attack
                move_list.push_back(Move(E8, C8, CASTLE_QUEEN));
            }
        }
    }
}

void Board::generate_sliding_moves(MoveList& move_list) {
    uint64_t friendly_pieces = side_to_move ? white_pieces : black_pieces;
    uint64_t enemy_pieces = side_to_move ? black_pieces : white_pieces;
    
    uint64_t rooks = side_to_move ? bitboards[WR] : bitboards[BR];
    while (rooks) {
        int from = __builtin_ctzll(rooks);
        uint64_t attacks = MagicBitboards::get_rook_attacks(from, all_pieces) & ~friendly_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            MoveType type = (enemy_pieces & (1ULL << to)) ? CAPTURE : NORMAL;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), type));
            attacks &= attacks - 1;
        }
        
        rooks &= rooks - 1;
    }
    
    uint64_t bishops = side_to_move ? bitboards[WB] : bitboards[BB];
    while (bishops) {
        int from = __builtin_ctzll(bishops);
        uint64_t attacks = MagicBitboards::get_bishop_attacks(from, all_pieces) & ~friendly_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            MoveType type = (enemy_pieces & (1ULL << to)) ? CAPTURE : NORMAL;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), type));
            attacks &= attacks - 1;
        }
        
        bishops &= bishops - 1;
    }
    
    uint64_t queens = side_to_move ? bitboards[WQ] : bitboards[BQ];
    while (queens) {
        int from = __builtin_ctzll(queens);
        uint64_t rook_attacks = MagicBitboards::get_rook_attacks(from, all_pieces);
        uint64_t bishop_attacks = MagicBitboards::get_bishop_attacks(from, all_pieces);
        uint64_t attacks = (rook_attacks | bishop_attacks) & ~friendly_pieces;
        
        while (attacks) {
            int to = __builtin_ctzll(attacks);
            MoveType type = (enemy_pieces & (1ULL << to)) ? CAPTURE : NORMAL;
            move_list.push_back(Move(static_cast<Square>(from), static_cast<Square>(to), type));
            attacks &= attacks - 1;
        }
        
        queens &= queens - 1;
    }
} 
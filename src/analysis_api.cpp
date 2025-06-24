#include "analysis_api.h"
#include <cmath>

std::string AnalysisApi::generate_analysis_json(const Board& board) {
    Multivector2D M_total = GeometricEvaluator::evaluate_position(board);
    nlohmann::json j;
    
    j["fen"] = board.to_fen_string();
    
    j["evaluation"]["final_score"] = GeometricEvaluator::get_final_score(M_total);
    j["evaluation"]["m_total_magnitude"] = calculate_multivector_magnitude(M_total);
    
    j["evaluation"]["components"]["scalar"] = M_total.get_scalar();
    j["evaluation"]["components"]["vector"]["x"] = M_total.get_vector().x;
    j["evaluation"]["components"]["vector"]["y"] = M_total.get_vector().y;
    j["evaluation"]["components"]["bivector"] = M_total.get_bivector().magnitude;
    
    j["visualizations"]["heatmap"] = generate_heatmap(M_total);
    j["visualizations"]["bivectors"] = generate_bivectors(board);
    
    return j.dump();
}

std::string AnalysisApi::square_to_string(Square square) {
    int file = square % 8;
    int rank = square / 8;
    return std::string(1, 'a' + file) + std::string(1, '1' + rank);
}

std::string AnalysisApi::piece_to_string(Piece piece, Square square) {
    char piece_char;
    switch (piece) {
        case WP: piece_char = 'P'; break;
        case WN: piece_char = 'N'; break;
        case WB: piece_char = 'B'; break;
        case WR: piece_char = 'R'; break;
        case WQ: piece_char = 'Q'; break;
        case WK: piece_char = 'K'; break;
        case BP: piece_char = 'p'; break;
        case BN: piece_char = 'n'; break;
        case BB: piece_char = 'b'; break;
        case BR: piece_char = 'r'; break;
        case BQ: piece_char = 'q'; break;
        case BK: piece_char = 'k'; break;
        default: piece_char = '?'; break;
    }
    
    return std::string(1, piece_char) + "-" + square_to_string(square);
}

float AnalysisApi::calculate_multivector_magnitude(const Multivector2D& mv) {
    float scalar = mv.get_scalar();
    Vector2D vector = mv.get_vector();
    float bivector = mv.get_bivector().magnitude;
    
    return std::sqrt(scalar * scalar + 
                    vector.x * vector.x + 
                    vector.y * vector.y + 
                    bivector * bivector);
}

float AnalysisApi::calculate_square_control_value(Square square, const Multivector2D& m_total) {
    Vector2D square_coords;
    square_coords.x = static_cast<float>(square % 8);
    square_coords.y = static_cast<float>(square / 8);
    
    Vector2D m_vector = m_total.get_vector();
    float distance = std::sqrt((square_coords.x - 3.5f) * (square_coords.x - 3.5f) + 
                              (square_coords.y - 3.5f) * (square_coords.y - 3.5f));
    
    float influence = (m_vector.x * square_coords.x + m_vector.y * square_coords.y) / (distance + 1.0f);
    influence += m_total.get_scalar() * 0.1f;
    influence += m_total.get_bivector().magnitude * 0.2f;
    
    return std::tanh(influence / 10.0f);
}

nlohmann::json AnalysisApi::generate_heatmap(const Multivector2D& m_total) {
    nlohmann::json heatmap = nlohmann::json::array();
    
    for (int square = 0; square < 64; square++) {
        Square sq = static_cast<Square>(square);
        float control_value = calculate_square_control_value(sq, m_total);
        
        nlohmann::json square_data;
        square_data["square"] = square_to_string(sq);
        square_data["value"] = control_value;
        
        heatmap.push_back(square_data);
    }
    
    return heatmap;
}

nlohmann::json AnalysisApi::generate_bivectors(const Board& board) {
    nlohmann::json bivectors = nlohmann::json::array();
    
    Piece sliding_pieces[] = {WB, WR, WQ, BB, BR, BQ};
    
    for (Piece piece_type : sliding_pieces) {
        uint64_t piece_bitboard = board.bitboards[piece_type];
        
        while (piece_bitboard) {
            int square_index = __builtin_ctzll(piece_bitboard);
            Square square = static_cast<Square>(square_index);
            
            PieceType type = GeometricEvaluator::piece_to_type(piece_type);
            Multivector2D influence = GeometricEvaluator::calculate_piece_influence(type, square, board);
            
            float bivector_strength = std::abs(influence.get_bivector().magnitude);
            
            if (bivector_strength > 0.01f) {
                nlohmann::json bivector_data;
                bivector_data["piece"] = piece_to_string(piece_type, square);
                bivector_data["strength"] = bivector_strength;
                
                nlohmann::json path = nlohmann::json::array();
                
                if (type == PieceType::BISHOP || type == PieceType::QUEEN) {
                    uint64_t attacks = MagicBitboards::get_bishop_attacks(square_index, board.all_pieces);
                    while (attacks) {
                        int attack_square = __builtin_ctzll(attacks);
                        path.push_back(square_to_string(static_cast<Square>(attack_square)));
                        attacks &= attacks - 1;
                    }
                }
                
                if (type == PieceType::ROOK || type == PieceType::QUEEN) {
                    uint64_t attacks = MagicBitboards::get_rook_attacks(square_index, board.all_pieces);
                    while (attacks) {
                        int attack_square = __builtin_ctzll(attacks);
                        path.push_back(square_to_string(static_cast<Square>(attack_square)));
                        attacks &= attacks - 1;
                    }
                }
                
                bivector_data["path"] = path;
                bivectors.push_back(bivector_data);
            }
            
            piece_bitboard &= piece_bitboard - 1;
        }
    }
    
    return bivectors;
} 
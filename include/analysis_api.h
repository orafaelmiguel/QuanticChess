#ifndef ANALYSIS_API_H
#define ANALYSIS_API_H

#include "geometric_evaluator.h"
#include "bitboard.h"
#include <nlohmann/json.hpp>
#include <string>

class AnalysisApi {
public:
    static std::string generate_analysis_json(const Board& board);
    
private:
    static std::string square_to_string(Square square);
    static std::string piece_to_string(Piece piece, Square square);
    static float calculate_multivector_magnitude(const Multivector2D& mv);
    static float calculate_square_control_value(Square square, const Multivector2D& m_total);
    static nlohmann::json generate_heatmap(const Multivector2D& m_total);
    static nlohmann::json generate_bivectors(const Board& board);
};

#endif // ANALYSIS_API_H 
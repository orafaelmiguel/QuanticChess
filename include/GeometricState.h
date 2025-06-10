#pragma once

#include <klein/klein.hpp>

enum class ChessSquare {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

struct GeometricState {
    kln::point position;
    kln::direction forward_direction;
    
    GeometricState(ChessSquare square);
    
    static kln::point square_to_point(ChessSquare square);
    static kln::direction get_white_pawn_direction();
    
    void update_position(ChessSquare new_square);
    kln::direction get_movement_vector(ChessSquare target_square) const;
};

inline GeometricState::GeometricState(ChessSquare square) 
    : position(square_to_point(square)), 
      forward_direction(get_white_pawn_direction()) {
}

inline kln::point GeometricState::square_to_point(ChessSquare square) {
    int file = static_cast<int>(square) % 8;
    int rank = static_cast<int>(square) / 8;
    return kln::point{static_cast<float>(file), static_cast<float>(rank), 0.0f};
}

inline kln::direction GeometricState::get_white_pawn_direction() {
    return kln::direction{0.0f, 1.0f, 0.0f};
}

inline void GeometricState::update_position(ChessSquare new_square) {
    position = square_to_point(new_square);
}

inline kln::direction GeometricState::get_movement_vector(ChessSquare target_square) const {
    kln::point target = square_to_point(target_square);
    kln::point diff = target - position;
    return kln::direction{diff.x(), diff.y(), diff.z()};
} 
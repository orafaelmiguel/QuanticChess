#include <iostream>
#include <string>
#include "GeometricState.h"

int main() {
    GeometricState white_pawn_e4(ChessSquare::E4);
    
    std::cout << "White pawn at e4:" << std::endl;
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
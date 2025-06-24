#include "geometric_algebra.h"

// Implementação dos construtores da classe Multivector2D
Multivector2D::Multivector2D() 
    : scalar_part(0.0f), vector_part(0.0f, 0.0f), bivector_part(0.0f) {}

Multivector2D::Multivector2D(float scalar) 
    : scalar_part(scalar), vector_part(0.0f, 0.0f), bivector_part(0.0f) {}

Multivector2D::Multivector2D(const Vector2D& vector) 
    : scalar_part(0.0f), vector_part(vector), bivector_part(0.0f) {}

Multivector2D::Multivector2D(const Bivector2D& bivector) 
    : scalar_part(0.0f), vector_part(0.0f, 0.0f), bivector_part(bivector) {}

Multivector2D::Multivector2D(float scalar, const Vector2D& vector, const Bivector2D& bivector) 
    : scalar_part(scalar), vector_part(vector), bivector_part(bivector) {}

// Implementação do operador de adição
Multivector2D Multivector2D::operator+(const Multivector2D& other) const {
    float new_scalar = scalar_part + other.scalar_part;
    Vector2D new_vector(vector_part.x + other.vector_part.x, 
                       vector_part.y + other.vector_part.y);
    Bivector2D new_bivector(bivector_part.magnitude + other.bivector_part.magnitude);
    
    return Multivector2D(new_scalar, new_vector, new_bivector);
}

// Implementação do operador de multiplicação por escalar
Multivector2D Multivector2D::operator*(float scalar) const {
    float new_scalar = scalar_part * scalar;
    Vector2D new_vector(vector_part.x * scalar, vector_part.y * scalar);
    Bivector2D new_bivector(bivector_part.magnitude * scalar);
    
    return Multivector2D(new_scalar, new_vector, new_bivector);
}

// Implementação do operador de multiplicação por escalar (comutativo)
Multivector2D operator*(float scalar, const Multivector2D& mv) {
    return mv * scalar;
}

// Implementação das funções auxiliares para vetores
float dot_product(const Vector2D& a, const Vector2D& b) {
    return a.x * b.x + a.y * b.y;
}

float wedge_product(const Vector2D& a, const Vector2D& b) {
    return a.x * b.y - a.y * b.x;
}

// Implementação do produto geométrico
Multivector2D geometric_product(const Multivector2D& a, const Multivector2D& b) {
    // Extrair componentes dos multivetores
    float a0 = a.get_scalar();
    Vector2D a_vec = a.get_vector();
    float a12 = a.get_bivector().magnitude;
    
    float b0 = b.get_scalar();
    Vector2D b_vec = b.get_vector();
    float b12 = b.get_bivector().magnitude;
    
    // Calcular o produto geométrico usando as regras da álgebra geométrica em 2D
    // (a0 + a1*e1 + a2*e2 + a12*e12) * (b0 + b1*e1 + b2*e2 + b12*e12)
    
    // Parte escalar: a0*b0 + a1*b1 + a2*b2 - a12*b12
    // (e1*e1 = 1, e2*e2 = 1, e12*e12 = -1)
    float result_scalar = a0 * b0 + dot_product(a_vec, b_vec) - a12 * b12;
    
    // Parte vetorial: a0*b_vec + b0*a_vec + a12*(b2*e1 - b1*e2) + b12*(a2*e1 - a1*e2)
    // (e12*e1 = e2, e12*e2 = -e1)
    Vector2D result_vector;
    result_vector.x = a0 * b_vec.x + b0 * a_vec.x + a12 * b_vec.y - b12 * a_vec.y;
    result_vector.y = a0 * b_vec.y + b0 * a_vec.y - a12 * b_vec.x + b12 * a_vec.x;
    
    // Parte bivetorial: a0*b12 + b0*a12 + a1*b2 - a2*b1
    // (e1*e2 = e12, e2*e1 = -e12)
    float result_bivector = a0 * b12 + b0 * a12 + wedge_product(a_vec, b_vec);
    
    return Multivector2D(result_scalar, result_vector, Bivector2D(result_bivector));
} 
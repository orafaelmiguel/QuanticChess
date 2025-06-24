#ifndef GEOMETRIC_ALGEBRA_H
#define GEOMETRIC_ALGEBRA_H

// Estrutura para representar um vetor 2D
struct Vector2D {
    float x;
    float y;
    
    // Construtores
    Vector2D() : x(0.0f), y(0.0f) {}
    Vector2D(float x_val, float y_val) : x(x_val), y(y_val) {}
};

// Estrutura para representar um bivetor em 2D (plano orientado)
struct Bivector2D {
    float magnitude;
    
    // Construtores
    Bivector2D() : magnitude(0.0f) {}
    Bivector2D(float mag) : magnitude(mag) {}
};

// Classe principal para multivetores em 2D
class Multivector2D {
private:
    float scalar_part;
    Vector2D vector_part;
    Bivector2D bivector_part;

public:
    // Construtores
    Multivector2D();
    Multivector2D(float scalar);
    Multivector2D(const Vector2D& vector);
    Multivector2D(const Bivector2D& bivector);
    Multivector2D(float scalar, const Vector2D& vector, const Bivector2D& bivector);
    
    // Métodos de acesso
    float get_scalar() const { return scalar_part; }
    Vector2D get_vector() const { return vector_part; }
    Bivector2D get_bivector() const { return bivector_part; }
    
    void set_scalar(float scalar) { scalar_part = scalar; }
    void set_vector(const Vector2D& vector) { vector_part = vector; }
    void set_bivector(const Bivector2D& bivector) { bivector_part = bivector; }
    
    // Operadores sobrecarregados
    Multivector2D operator+(const Multivector2D& other) const;
    Multivector2D operator*(float scalar) const;
    
    // Operador de multiplicação por escalar (comutativo)
    friend Multivector2D operator*(float scalar, const Multivector2D& mv);
};

// Função para calcular o produto geométrico entre dois multivetores
Multivector2D geometric_product(const Multivector2D& a, const Multivector2D& b);

// Funções auxiliares para operações com vetores
float dot_product(const Vector2D& a, const Vector2D& b);
float wedge_product(const Vector2D& a, const Vector2D& b);

#endif // GEOMETRIC_ALGEBRA_H 
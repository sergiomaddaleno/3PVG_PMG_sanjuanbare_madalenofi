
#include "light.h"

Light::Light()
    : type_(0),                         // Inicializamos type_ a 0
    enabled_(0),                        // false por defecto
    dir_(0.0f, 1.0f, 0.0f),            // Dirección por defecto hacia abajo
    diff_color_(1.0f, 1.0f, 1.0f),      // Color difuso blanco
    spec_color_(1.0f, 1.0f, 1.0f),      // Color especular blanco
    light_color_(1.0f, 0.0f, 0.0f),     // Color de luz blanco
    ambient_color(0.1f, 0.1f, 0.1f),    // Color ambiental tenue
    linear_att(0.7f),                   // Atenuación lineal por defecto
    quad_att_(1.8f),                    // Atenuación cuadrática por defecto
    constant_att_(1.0f),                // Atenuación constante por defecto
    shininess_(32.0f),                  // Brillo por defecto
    strength_(1.0f),                    // Intensidad por defecto
    spot_angle(45.0f),                  // Ángulo del spot por defecto
    camera_pos_{ 0.0f, 0.0f, 0.0f },    // Posición de cámara en (0,0,0)
    light_linear_(0.09f),               // Atenuación lineal personalizada
    light_quadratic_(0.032f)            // Atenuación cuadrática personalizada
{}


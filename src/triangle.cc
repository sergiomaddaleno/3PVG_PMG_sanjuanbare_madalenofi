
#include "..\include\triangle.h"
#include <../deps/linmath.h>

std::optional<Triangle> Triangle::make(float size_, Color color)
{

	PointsTriangle t[3];

    switch (color) {
    case Color::Red:
        t[0] = { -0.6f * size_, -0.4f * size_, 1.0f, 0.0f, 0.0f }; 
        t[1] = { 0.6f * size_, -0.4f * size_, 1.0f, 0.0f, 0.0f };  
        t[2] = { 0.0f,  0.6f * size_, 1.0f, 0.0f, 0.0f };
        break;

    case Color::Green:
        t[0] = { -0.6f * size_, -0.4f * size_, 0.0f, 1.0f, 0.0f }; 
        t[1] = { 0.6f * size_, -0.4f * size_, 0.0f, 1.0f, 0.0f };
        t[2] = { 0.0f,  0.6f * size_, 0.0f, 1.0f, 0.0f }; 
        break;

    case Color::Blue:
        t[0] = { -0.6f * size_, -0.4f * size_, 0.0f, 0.0f, 1.0f };
        t[1] = { 0.6f * size_, -0.4f * size_, 0.0f, 0.0f, 1.0f };
        t[2] = { 0.0f,  0.6f * size_, 0.0f, 0.0f, 1.0f };
        break;

    case Color::Yellow:
        t[0] = { -0.6f * size_, -0.4f * size_, 1.0f, 1.0f, 0.0f };
        t[1] = { 0.6f * size_, -0.4f * size_, 1.0f, 1.0f, 0.0f }; 
        t[2] = { 0.0f,  0.6f * size_, 1.0f, 1.0f, 0.0f }; 
        break;
    }

	return std::optional<Triangle>( t );
}


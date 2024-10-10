#include "..\include\tija\pr1_triangle.h"

#include <tija/linmath.h>

std::optional<Triangle> Triangle::make(float size_, const std::string& name)
{

	PointsTriangles t[3];
	ColorTriangle c[3];

	

	if (name == "green") {
		c[0] = { 0.0f, 1.0f, 0.0f };
		c[1] = { 0.0f, 1.0f, 0.0f };
		c[2] = { 0.0f, 1.0f, 0.0f };

		t[0] = { -0.6f * size_, -0.4f * size_, c[0].r, c[0].g, c[0].b };
		t[1] = { 0.6f * size_, -0.4f * size_, c[1].r, c[1].g, c[1].b };
		t[2] = { 0.f,  0.6f * size_, c[2].r, c[2].g, c[2].b };
	}

	return std::optional<Triangle>( t , c);
}



#ifndef PRT
#define PRT

#include <optional>
#include <string>
#include <../deps/linmath.h>

class Triangle {

public:
	 
	struct PointsTriangle {
		float x, y;
		float r, g, b;
	};

	enum Color {
		Red,
		Green,
		Blue,
		Yellow,
		Other,
	};

	PointsTriangle my_triangle[3];   

	Triangle(PointsTriangle t[3]) {
		my_triangle[0] = t[0];
		my_triangle[1] = t[1];
		my_triangle[2] = t[2];
	}

	static std::optional<Triangle> make(float size_, Color color);

	//PointsTriangle GetVertices() { }
};

#endif // PRT

#ifndef PRT
#define PRT

#include <optional>
#include <string>
#include <tija/linmath.h>

class Triangle {

public:
	 
	struct PointsTriangles {
		float x, y;
	} ;

	struct ColorTriangle {
		float r, g, b;
	};
	
	ColorTriangle triangle_color_[3];
	
	PointsTriangles my_triangle[3];   

	Triangle();

	Triangle(PointsTriangles t[3], ColorTriangle c[3]) { my_triangle[0] = t[0]; 
														my_triangle[1] = t[1]; 
														my_triangle[2] = t[2];
														triangle_color_[0] = c[0];
														triangle_color_[1] = c[1];
														triangle_color_[2] = c[2];
														
	};

	static std::optional<Triangle> make(float size_ = 1.0f, const std::string& name);

};

#endif // PRT

#ifndef PR1
#define PR1

#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <optional>
#include <string>


class Triangle {

public:
	 
	struct PointsTriangles {
		float x, y;
		float r, g, b;
	} ;
	
	PointsTriangles my_triangle[3];   

	void make(float size_=1.0f);

	Triangle() {};

};

#endif // PR1
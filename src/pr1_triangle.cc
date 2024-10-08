#include "..\include\tija\pr1_triangle.h"

   /*
	
   */

void Triangle::make(float size_)
{
       
	my_triangle[0] = { -0.6f * size_, -0.4f * size_, 1.f, 0.f, 0.f };
	my_triangle[1] = { 0.6f * size_, -0.4f * size_, 0.f, 1.f, 0.f };
	my_triangle[2] = { 0.f,  0.6f * size_, 0.f, 0.f, 1.f };
    
 

}


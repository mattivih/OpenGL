#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>

using namespace glm;

int main (void) 
{
	glm::vec3 vec(1.0f, 2.0f, 3.0f);
	printf("Vector %f, %f, %f", vec.x, vec.y, vec.z);
	return 0;
}

#ifndef _CURVE_H_
#define _CURVE_H_ 

#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "Node.h"

class Curve
{
    private:
	GLuint vao, vbo;
	public:
	Curve(GLuint);
	~Curve();
	void draw(glm::mat4);
	GLuint prog;
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> vertices;
	glm::vec3 iterator();
	int index;

	void inter(float);
};

#endif

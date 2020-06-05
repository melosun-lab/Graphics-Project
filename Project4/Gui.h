#ifndef _GUI_H_
#define _GUI_H_ 

#include <OpenGL/gl3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>
#include "Window.h"

#include "Object.h"



class Gui : public Object
{
public:
	GLuint texture, vao, vbo, ebo, program;
	std::vector<glm::vec3> points;//center of quad
	std::vector<unsigned int> faces;

	Gui(GLuint, glm::vec2, glm::vec2);
	~Gui();
	void loadToVAO();
	void draw();
	void update();
	unsigned char* loadPPM(const char* filename, int& width, int& height);
	void loadTexture();

};
#endif

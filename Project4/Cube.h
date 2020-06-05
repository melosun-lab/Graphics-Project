#ifndef _CUBE_H_
#define _CUBE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "Object.h"

class Cube : public Object
{
private:
	GLuint vao;
	GLuint vbos[2];
	GLuint cubemapTexture;
public:
	Cube(float size);
	~Cube();

	void draw();
	void update();

	void spin(float deg);
	unsigned char* loadPPM(const char*, int&, int&);
	void loadCubemap();
	void setScale(float x, float y, float z);
};

#endif
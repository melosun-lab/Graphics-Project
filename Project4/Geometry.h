#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_ 

#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "Node.h"

class Geometry : public Node {
public: 
	GLuint vao, vbo, vbon, vbot, ebo, fbo;

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> npoints;
	std::vector<glm::vec3> textures;
	std::vector<unsigned int> faces;
	std::vector<unsigned int> verticesI;
	std::vector<unsigned int> normalsI;
	std::vector<unsigned int> texturesI;

	GLuint prog;
	GLuint texture;

	glm::mat4 model;
	glm::vec3 color;

	glm::mat4 M;

	int type;
	float textSample;

	Geometry(std::string, float, GLuint, int, int);
    ~Geometry();
	void setMatrix(glm::mat4);
	void initialization(std::string, float);
	void draw(glm::mat4);
	void update();
	unsigned char* loadPPM(const char*, int&, int&);
	void loadTexture();
};

#endif

#include "Curve.h"

//Curve::Curve(GLuint program, std::vector<glm::vec3> cpoints){
Curve::Curve(GLuint program) {

	/*first*/
	points.push_back(glm::vec3(0, 0, 0));
	points.push_back(glm::vec3(10, 10, 0));
	points.push_back(glm::vec3(10, 10, 0));
	points.push_back(glm::vec3(15, 4, 0));
	/*second*/
	points.push_back(glm::vec3(19, 18, 17));
	points.push_back(glm::vec3(11, 6, 8));
	/*third*/
	points.push_back(glm::vec3(16, 14, 13));
	points.push_back(glm::vec3(15, 12, 10));
	/*fourth*/
	points.push_back(glm::vec3(13, 4, 15));
	points.push_back(glm::vec3(8, 8, 18));

	inter(0.005);
	index = 0;
}

Curve::~Curve(){}


void Curve::inter(float t) {

	glm::mat4 benz{ -1,3,-3,1,3,-6,3,0,-3,3,0,0,1,0,0,0 };
	/*first*/
	glm::mat4 pointM{ points[0].x, points[0].y, points[0].z, 0,
	points[1].x, points[1].y, points[1].z, 0,
	points[2].x, points[2].y, points[2].z, 0,
	points[3].x, points[3].y, points[3].z, 0 };

	for (float i = 0; i < 1; i+=t) {
		glm::vec4 param = glm::vec4(i * i * i, i * i, i, 1);
		glm::vec4 tmp = pointM * benz * param;
		vertices.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		//std::cout << tmp.x << " " << tmp.y << " " << tmp.z << std::endl;
	}
	/*second*/
	pointM = glm::mat4{ points[3].x, points[3].y, points[3].z, 0,
	2 * points[3].x - points[2].x, 2 * points[3].y - points[2].y, 2 * points[3].z - points[2].z, 0,
	points[4].x, points[4].y, points[4].z, 0,
	points[5].x, points[5].y, points[5].z, 0 };
	
	for (float i = 0; i < 1; i += t) {
		glm::vec4 param = glm::vec4(i * i * i, i * i, i, 1);
		glm::vec4 tmp = pointM * benz * param;
		vertices.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		//std::cout << tmp.x << " " << tmp.y << " " << tmp.z << std::endl;
	}
	/*third*/
	pointM = glm::mat4{ points[5].x, points[5].y, points[5].z, 0,
	2 * points[5].x - points[4].x, 2 * points[5].y - points[4].y, 2 * points[5].z - points[4].z, 0,
	points[6].x, points[6].y, points[6].z, 0,
	points[7].x, points[7].y, points[7].z, 0 };

	for (float i = 0; i < 1; i += t) {
		glm::vec4 param = glm::vec4(i * i * i, i * i, i, 1);
		glm::vec4 tmp = pointM * benz * param;
		vertices.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		//std::cout << tmp.x << " " << tmp.y << " " << tmp.z << std::endl;
	}
	/*fourth*/
	pointM = glm::mat4{ points[7].x, points[7].y, points[7].z, 0,
	2 * points[7].x - points[6].x, 2 * points[7].y - points[6].y, 2 * points[7].z - points[6].z, 0,
	points[8].x, points[8].y, points[8].z, 0,
	points[9].x, points[9].y, points[9].z, 0 };

	for (float i = 0; i < 1; i += t) {
		glm::vec4 param = glm::vec4(i * i * i, i * i, i, 1);
		glm::vec4 tmp = pointM * benz * param;
		vertices.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		//std::cout << tmp.x << " " << tmp.y << " " << tmp.z << std::endl;
	}
	/*fifth*/
	pointM = glm::mat4{ points[9].x, points[9].y, points[9].z, 0,
	2 * points[9].x - points[8].x, 2 * points[9].y - points[8].y, 2 * points[9].z - points[8].z, 0,
	2 * points[0].x - points[1].x, 2 * points[0].y - points[1].y, 2 * points[0].z - points[1].z, 0,
	points[0].x, points[0].y, points[0].z, 0 };

	for (float i = 0; i < 1; i += t) {
		glm::vec4 param = glm::vec4(i * i * i, i * i, i, 1);
		glm::vec4 tmp = pointM * benz * param;
		vertices.push_back(glm::vec3(tmp.x, tmp.y, tmp.z));
		//std::cout << tmp.x << " " << tmp.y << " " << tmp.z << std::endl;
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
		vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void Curve::draw(glm::mat4 C) {

	glUseProgram(prog);

	glBindVertexArray(vao);
	glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
	glBindVertexArray(0);
};

glm::vec3 Curve::iterator() {
	if (index < vertices.size()) {
		return vertices[++index];
	}
	else {
		index = 0;
		return vertices[index];
	}
}
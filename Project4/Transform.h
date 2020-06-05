#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_ 

#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Curve.h"
#include "Node.h"
class Transform : public Node {
public:
	glm::mat4 M;
	glm::mat4 rotationM;
	glm::mat4 scrollM;
	glm::mat4 worldM;
	std::vector<Node*> list;
	std::vector<Curve*> curves;

	float degree;
	bool forward;
	bool domove;
	glm::vec3 axis;
	glm::vec3 rotationCenter;
	float threshold;

	void addChild(Node*);
	void addChildCurve(Curve*);
	void removeChild(Node*);
	void draw(glm::mat4);
	void update();
	void rotation(float, glm::vec3);
	void scroll(float);
	void updateWorld(glm::vec3);

	Transform(glm::mat4, bool, float, bool, glm::vec3, glm::vec3, float);
	~Transform();

};

#endif

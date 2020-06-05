#ifndef _NODE_H_
#define _NODE_H_

#include <glm/glm.hpp>
#include <iostream>

class Node
{
public:
	virtual void draw(glm::mat4 C) = 0;
	virtual void update() = 0;
};

#endif
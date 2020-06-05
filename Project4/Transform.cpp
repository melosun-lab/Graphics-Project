#include "Transform.h"

Transform::Transform(glm::mat4 C, bool move , float deg, bool isForward, glm::vec3 rot, glm::vec3 rotCenter, float thre) {
	domove = move;
	axis = rot;
	degree = deg;
	forward = isForward;
	rotationCenter = rotCenter;
	threshold = thre;
	M = C;
	rotationM = glm::mat4(1.0f);
	scrollM = glm::mat4(1.0f);
	worldM = glm::mat4(1.0f);
}

Transform::~Transform(){
	/*
	for (int i = 0; i < list.size(); i++) {
		delete list[i];
	}
	*/
}

void Transform::addChild(Node* node) {
	list.push_back(node);
};

void Transform::addChildCurve(Curve* curve){
	curves.push_back(curve);
}

void Transform::removeChild(Node* node) {
	for (unsigned int i = 0; i < list.size(); i++ ) {
		if (list[i] == node) {
			list.erase(list.begin() + i);
			delete node;
		}
	}
};

void Transform::draw(glm::mat4 C) {
	glm::mat4 matrix = M;
	//matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-rotationCenter.x, -rotationCenter.y, -rotationCenter.z)) * matrix;																																			//matrix = C * scrollM * rotationM * glm::rotate(glm::mat4(1.0f), degree, axis) * matrix;
	//matrix = worldM * scrollM * rotationM * glm::rotate(glm::mat4(1.0f), degree, axis) * matrix;
	//matrix = glm::translate(glm::mat4(1.0f), glm::vec3(rotationCenter.x, rotationCenter.y, rotationCenter.z)) * matrix;
	matrix = C * matrix;
	//std::cout << glm::to_string(matrix) << std::endl;
	for (unsigned int i = 0; i < list.size(); i++) {\
        list[i]->draw(matrix);
	}
};

void Transform::update() {
	if (domove) {
		if (forward) {
			degree += 0.002f;
		}
		else {
			degree -= 0.002f;
		}

		//std::cout << degree << std::endl;

		if (degree > threshold) {
			//std::cout << "here" << std::endl;
			forward = false;
		}
		if (degree < -threshold) {
			//std::cout << "here" << std::endl;
			forward = true;
		}
	}
	for (unsigned int i = 0; i < list.size(); i++) {
		list[i]->update();
	}
}

void Transform::rotation(float rad, glm::vec3 rotationAxis) {
// need to control draw
	rotationM = glm::rotate(glm::mat4(1.0f), rad, rotationAxis) * rotationM;
}

void Transform::scroll(float z) {
	// TODO
	if (z > 0) {
		scrollM = glm::scale(glm::mat4(1.0f), glm::vec3(1.1, 1.1, 1.1)) * scrollM;
	}
	else {
		scrollM = glm::scale(glm::mat4(1.0f), glm::vec3(0.9, 0.9, 0.9)) * scrollM;
	}
		//glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, z)) * scrollM;
}

void Transform::updateWorld(glm::vec3 newPos) {
	worldM = glm::mat4{1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	newPos.x,newPos.y,newPos.z,1};
}

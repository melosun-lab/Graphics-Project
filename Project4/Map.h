#ifndef _MAP_H_
#define _MAP_H_

#include <OpenGL/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cstdlib>
#include <stdlib.h> 
#include <time.h>  

class Map
{
public:
	Map();
	void printMap();
	void resetMap();

	std::vector<std::vector<char>> charMap;
};

#endif

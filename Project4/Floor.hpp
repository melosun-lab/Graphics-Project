//
//  Floor.hpp
//  167 Final Project
//
//  Created by 孙楚华 on 3/15/20.
//  Copyright © 2020 孙楚华. All rights reserved.
//

#ifndef Floor_hpp
#define Floor_hpp

#include <stdio.h>


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

class Floor : public Object
{
private:
    GLuint vao, vbo, vbo_n, vbo_t;
    GLuint ebo;
    GLuint texture;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> points_norm;
    std::vector<unsigned int> faces;
    std::vector<glm::vec3> texcoord;
public:
    Floor(float size);
    ~Floor();

    void draw();
    void update();
    
    unsigned char* loadPPM(const char*, int&, int&);
    void setScale(float x, float y, float z);
    void loadTexture();
};


#endif /* Floor_hpp */

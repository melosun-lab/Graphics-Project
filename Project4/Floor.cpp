//
//  Floor.cpp
//  167 Final Project
//
//  Created by 孙楚华 on 3/15/20.
//  Copyright © 2020 孙楚华. All rights reserved.
//

#include "Floor.hpp"

Floor::Floor(float size)
{
    // Model matrix. Since the original size of the cube is 2, in order to
    // have a cube of some size, we need to scale the cube by size / 2.
    model = glm::scale(glm::vec3(size / 2.f));

    // The color of the cube. Try setting it to something else!
    color = glm::vec3(1.0f, 0.95f, 0.1f);
    
    loadTexture();

    /*
     * Cube indices used below.
     *    4----7
     *   /|   /|
     *  0-+--3 |
     *  | 5--+-6
     *  |/   |/
     *  1----2
     *
     */

     // The 8 vertices of a cube.
    std::vector<glm::vec3> vertices
    {
        glm::vec3(-1, 1, 1),
        glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(-1, 1, -1),
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1)
    };

    // Each ivec3(v1, v2, v3) define a triangle consists of vertices v1, v2
    // and v3 in counter-clockwise order.
    std::vector<glm::ivec3> indices
    {
        // Front face.
        glm::ivec3(0, 1, 2),
        glm::ivec3(2, 3, 0),
        // Back face.
        glm::ivec3(7, 6, 5),
        glm::ivec3(5, 4, 7),
        // Right face.
        glm::ivec3(3, 2, 6),
        glm::ivec3(6, 7, 3),
        // Left face.
        glm::ivec3(4, 5, 1),
        glm::ivec3(1, 0, 4),
        // Top face.
        glm::ivec3(4, 0, 3),
        glm::ivec3(3, 7, 4),
        // Bottom face.
        glm::ivec3(1, 5, 6),
        glm::ivec3(6, 2, 1),
    };
    
    std::vector<glm::vec3> texCoord
    {
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0),
        glm::vec3(1.0, 1.0, 0.0),
        glm::vec3(1.0, 1.0, 0.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 0.0),
    };

    points = vertices;
    points_norm = points;
    for(unsigned int i = 0; i < indices.size(); i++){
        faces.push_back(int(indices[i].x));
        faces.push_back(int(indices[i].y));
        faces.push_back(int(indices[i].z));
    }
    
    texcoord = texCoord;
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &vbo_t);
    glGenBuffers(1, &vbo_n);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
        points.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_t);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* texcoord.size(),
        texcoord.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_n);
    // Pass in the data.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points_norm.size(),
        points_norm.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * faces.size(), faces.data(), GL_STATIC_DRAW);

    // Unbind from the VBO.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind from the VAO.
    glBindVertexArray(0);

}

Floor::~Floor()
{
    glDeleteVertexArrays(1, &vao);
}

void Floor::draw()
{
    glActiveTexture(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, (int)faces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Floor::update()
{
}

unsigned char* Floor::loadPPM(const char* filename, int& width, int& height)
{
    const int BUFSIZE = 128;
    FILE* fp;
    unsigned int read;
    unsigned char* rawData;
    char buf[3][BUFSIZE];
    char* retval_fgets;
    size_t retval_sscanf;

    if ((fp = fopen(filename, "rb")) == NULL)
    {
        std::cerr << "error reading ppm file, could not locate " << filename << std::endl;
        width = 0;
        height = 0;
        return NULL;
    }

    // Read magic number:
    retval_fgets = fgets(buf[0], BUFSIZE, fp);

    // Read width and height:
    do
    {
        retval_fgets = fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');
    retval_sscanf = sscanf(buf[0], "%s %s", buf[1], buf[2]);
    width = atoi(buf[1]);
    height = atoi(buf[2]);

    // Read maxval:
    do
    {
        retval_fgets = fgets(buf[0], BUFSIZE, fp);
    } while (buf[0][0] == '#');

    // Read image data:
    rawData = new unsigned char[width * height * 3];
    read = fread(rawData, width * height * 3, 1, fp);
    fclose(fp);
    if (read != 1)
    {
        std::cerr << "error parsing ppm file, incomplete data" << std::endl;
        delete[] rawData;
        width = 0;
        height = 0;
        return NULL;
    }

    return rawData;
}

void Floor::setScale(float x, float y, float z) {
    model = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Floor::loadTexture() {
    //glActiveTexture(GL_TEXTURE0);
    //glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);

    unsigned char* tdata;  // texture pixel data
    int twidth, theight;   // texture width/height [pixels]

    tdata = loadPPM("terrain.ppm", twidth, theight);
    if (tdata)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
        glGenerateMipmap(GL_TEXTURE_2D);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

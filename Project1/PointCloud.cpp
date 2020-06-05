#include "PointCloud.h"
#include <iostream>
using namespace std;


PointCloud::PointCloud(std::string objFilename, GLfloat pointSize)
	: pointSize(pointSize)
{
	/*
	 * TODO: Section 2: Currently, all the points are hard coded below.
	 * Modify this to read points from an obj file.
	 * Don't forget to load in the object normals for normal coloring as well
	 */
	points =
	{
		glm::vec3(-2.5, 2.5, 2.5),
		glm::vec3(-2.5, -2.5, 2.5),
		glm::vec3(2.5, -2.5, 2.5),
		glm::vec3(2.5, 2.5, 2.5),
		glm::vec3(-2.5, 2.5, -2.5),
		glm::vec3(-2.5, -2.5, -2.5),
		glm::vec3(2.5, -2.5, -2.5),
		glm::vec3(2.5, 2.5, -2.5)
	};
    
    points = {};
    
    FILE* fp;
    float v_x,v_y,v_z;
    int c_1,c_2;
    
    fp = fopen("bunny.obj","rb");
    //while(!feof(fp)){
        if(fp == NULL){exit(-1);}
    
        c_1 = fgetc(fp);
        c_2 = fgetc(fp);
    cout << c_1 << endl;
    cout << "hiiiiiiiiiii" << flush;
        //if(c_1 == 'v' && c_2 == ' '){
         //   fscanf(fp, "%f %f %f", &v_x, &v_y, &v_z);
        //}
    //}
    //std::cout << v_x << std::endl;
    fclose(fp);
    
    
    

	/*
	 * TODO: Section 4, you will need to normalize the object to fit in the
	 * screen.
	 */

	 // Set the model matrix to an identity matrix. 
	model = glm::mat4(1);
	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	// Bind to the VAO.
	// This tells OpenGL which data it should be paying attention to
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	/*
	 * TODO: Section 2 and 3. 	 
	 * Following the above example but for vertex normals, 
	 * 1) Generate a new vertex bufferbuffer,
	 * 2) Bind it as a GL_ARRAY_BUFFER type, 
	 * 3) Pass in the data 
	 * 4) Enable the next attribute array (which is 1)
	 * 5) Tell it how to process each vertex using glVertexAttribPointer
	 */

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
}

PointCloud::~PointCloud()
{
	// Delete the VBO and the VAO.
	// Failure to delete your VAOs, VBOs and other data given to OpenGL
	// is dangerous and may slow your program and cause memory leaks
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void PointCloud::draw()
{
	// Bind to the VAO.
	glBindVertexArray(vao);
	// Set point size.
	glPointSize(pointSize);
	// Draw points 
	glDrawArrays(GL_POINTS, 0, points.size());
	// Unbind from the VAO.
	glBindVertexArray(0);
}

void PointCloud::update()
{
	// Spin the cube by 1 degree.
	spin(0.1f);
	/*
	 * TODO: Section 3: Modify this function to spin the dragon and bunny about
	 * different axes. Look at the spin function for an idea
	 */
}

void PointCloud::updatePointSize(GLfloat size)
{
	/*
	 * TODO: Section 3: Implement this function to adjust the point size.
	 */
}

void PointCloud::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = glm::rotate(model, glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}


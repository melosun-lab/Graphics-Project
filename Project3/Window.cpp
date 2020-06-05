#include "Window.h"
#include <stdlib.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <math.h>   // include math functions, such as sin, cos, M_PI
#include <iostream> // allow c++ style console printouts
/* 
 * Declare your variables below. Unnamed namespace is used here to avoid 
 * declaring global or static variables.
 */
namespace
{
	int width, height;
	std::string windowTitle("GLFW Starter Project");

	// Boolean switches
    Curve* bezier;
    Cube* cubePoints;
    unsigned int cubemapTexture;

	//PointCloud* currentObj;  // The object currently displaying.

    Cube* currentObj;

    glm::vec3 eye(0, 0, 30);
	glm::vec3 center(0, 0, 0); // The point we are looking at.
	glm::vec3 up(0, 1, 0); // The up direction of the camera.
    glm::vec3 center_position = glm::vec3(0.0f);
	float fovy = 60;
	float near = 1;
	float far = 1000;

    bool pressed = false;
    bool camera = true;
    bool move = true;
    float start_x = 0;
    float start_y = 0;
    float current_x = 0;
    float current_y = 0;
    int threshold = 0;

    Transform* Army;
    Transform* Root;
    Transform* Robot;
    Transform* head_t;
    Transform* body_t;
    Transform* eyeball_left;
    Transform* eyeball_right;
    Transform* arm_left;
    Transform* arm_right;
    Transform* antenna_left;
    Transform* antenna_right;
    Transform* leg_left;
    Transform* leg_right;
    
    Geometry* body;
    Geometry* head;
    Geometry* eyeball;
    Geometry* antenna;
    Geometry* limb;

    std::vector<Transform*> army;
    std::vector<Transform*> temps;
	
    glm::mat4 view = glm::lookAt(eye, center, up); // View matrix, defined by eye, center and up.
	glm::mat4 projection; // Projection matrix.

    GLuint skybox;
	GLuint projectionLoc; // Location of projection in shader.
	GLuint viewLoc; // Location of view in shader.
	GLuint modelLoc; // Location of model in shader.
	GLuint colorLoc; // Location of color in shader.
    GLuint modeLoc;

    std::vector<std::string> faces
    {
        "s_right.jpg",
        "s_left.jpg",
        "s_top.jpg",
        "s_bottom.jpg",
        "s_front.jpg",
        "s_back.jpg"
    };
    
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> direction;
};

bool Window::initializeProgram()
{
	// Create a shader program with a vertex shader and a fragment shader.
	//program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
    skybox = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");
	// DO NOT MODIFY THESE SHADER PROGRAMS
	// Check the shader programs.
    if (!skybox)
    {
        std::cerr << "Failed to initialize skybox program" << std::endl;
        return false;
    }
    
    loadTexture();

	// Activate the shader program.
	//glUseProgram(program);
	// Get the locations of uniform variables.
    glUseProgram(skybox);
	projectionLoc = glGetUniformLocation(skybox, "projection");
	viewLoc = glGetUniformLocation(skybox, "view");
	modelLoc = glGetUniformLocation(skybox, "model");
	colorLoc = glGetUniformLocation(skybox, "color");
    modeLoc = glGetUniformLocation(skybox, "mode");
    
    return true;
}

bool Window::initializeObjects()
{
    bezier = new Curve();
    points = bezier->get_points();
    for(unsigned int i = 0; i < points.size() - 1; i++){
        direction.push_back(points[i+1] - points[i]);
    }
    direction.push_back(points[0] - points[points.size()-1]);
    
    cubePoints = new Cube(1000.0f);
    currentObj = cubePoints;

    cubePoints->setID(cubemapTexture);
    
    body = new Geometry(skybox, "body_s.obj");
    head = new Geometry(skybox, "head_s.obj");
    limb = new Geometry(skybox, "limb_s.obj");
    eyeball = new Geometry(skybox, "eyeball_s.obj");
    antenna = new Geometry(skybox, "antenna_s.obj");
    
    initialzieTransfroms();
    
    Root = new Transform(glm::mat4(1.0f));
    
    Army = new Transform(glm::mat4(1.0f));
    
    for(unsigned int i = 0; i < 5; i++){
        for(unsigned int j = 0; j < 5; j++){
            Transform* temp = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(float(i) * 3.0f - 6.0f,0.0f,float(j) * 3.0f - 6.0f)));
            temp->addChild(Robot);
            Army->addChild(temp);
        }
    }
    
    Root->addChild(Army);
    
	return true;
}

void Window::initialzieTransfroms(){
    Robot = new Transform(glm::mat4(1.0f)); //root
    
    head_t = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,1.0f,0.0f)));
    body_t = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.01f,0.0f)));
    
    eyeball_left = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.3f,1.25f,0.9f)));
    eyeball_right = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f,1.25f,0.9f)));
    
    antenna_left = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.3f,1.3f,0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, -1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)));
    antenna_right = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f,1.3f,0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)));
    antenna_left->set_spin(glm::vec3(0.0f, 0.0f, 1.0f));
    antenna_right->set_spin(glm::vec3(0.0f, 0.0f, -1.0f));
    antenna_left->set_vec(glm::vec3(0.0f, 0.0f, 0.0f));
    antenna_right->set_vec(glm::vec3(0.0f, 0.0f, 0.0f));
    antenna_left->set_limit(5.0);
    antenna_right->set_limit(5.0);
    
    arm_left = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(1.15f,0.3f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.25f, 1.25f, 1.25f)));
    arm_right = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-1.15f,0.3f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.25f, 1.25f, 1.25f)));
    arm_left->set_spin(glm::vec3(0.0f, 1.0f, 0.0f));
    arm_right->set_spin(glm::vec3(0.0f, 1.0f, 0.0f));
    arm_left->set_vec(glm::vec3(0.0, -0.8, 0.0));
    arm_right->set_vec(glm::vec3(0.0, -0.8, 0.0));
    
    leg_left = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.5f,-1.5f,0.0f)));
    leg_right = new Transform(glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f,-1.5f,0.0f)));
    leg_left->set_spin(glm::vec3(1.0f, 0.0f, 0.0f));
    leg_right->set_spin(glm::vec3(-1.0f, 0.0f, 0.0f));
    leg_left->set_vec(glm::vec3(0.0, 0.5, 0.0));
    leg_right->set_vec(glm::vec3(0.0, 0.5, 0.0));
    
    Robot->addChild(head_t);
    Robot->addChild(body_t);
    Robot->addChild(eyeball_left);
    Robot->addChild(eyeball_right);
    Robot->addChild(antenna_left);
    Robot->addChild(antenna_right);
    Robot->addChild(arm_left);
    Robot->addChild(arm_right);
    Robot->addChild(leg_left);
    Robot->addChild(leg_right);
    head_t->addChild(head);
    body_t->addChild(body);
    eyeball_left->addChild(eyeball);
    eyeball_right->addChild(eyeball);
    antenna_left->addChild(antenna);
    antenna_right->addChild(antenna);
    arm_left->addChild(limb);
    arm_right->addChild(limb);
    leg_left->addChild(limb);
    leg_right->addChild(limb);
    
}

void Window::cleanUp()
{
    delete cubePoints;
    delete body;
    delete head;
    delete limb;
    delete eyeball;
    delete antenna;
    delete Robot;
    delete head_t;
    delete body_t;
    delete eyeball_left;
    delete eyeball_right;
    delete arm_left;
    delete arm_right;
    delete antenna_left;
    delete antenna_right;
    delete leg_left;
    delete leg_right;
    delete Root;
    delete Army;
    delete bezier;

	// Delete the shader programs.
    glDeleteProgram(skybox);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);
	
	// Initialize the quad that will be textured with your image
	// The quad must be made with the window

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int w, int h)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height);
#endif
	width = w;
	height = h;

	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	projection = glm::perspective(glm::radians(fovy),
		(float)width / (float)height, near, far);
}

void Window::idleCallback()
{
	// Perform any updates as necessary. 
	cubePoints->update();
    Root->update();
    if(move){
        if(threshold % 10 == 0){
            Root->set_translate(direction[threshold/10]);
            center_position += direction[threshold/10];
        }
        threshold += 1;
        if(threshold == direction.size()*10){
            threshold = 0;
        }
    }
    Root->set_values(center_position);
}

void Window::displayCallback(GLFWwindow* window)
{
    //glUseProgram(program);
    glUseProgram(skybox);
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Specify the values of the uniform variables we are going to use.
    /*
     * TODO: Section 3 and 4: Modify the code here to draw both the bunny and
     * the dragon
     * Note that the model matrix sent to the shader belongs only
     * to what object the currentObj ptr is pointing to. You will need to
     * use another call to glUniformMatrix4fv to change the model matrix
     * data being sent to the vertex shader before you draw the other object
     */

    glm::mat4 model = cubePoints->getModel();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(modeLoc, 0.0f);
    cubePoints->draw();
    
    
    model = bezier->getModel();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1f(modeLoc, 2.0f);
    bezier->draw();
    
    
    glUniform1f(modeLoc, 1.0f);
    Root->draw(glm::mat4(1.0f));
    
    // Gets events, including input such as keyboard and mouse or window resizing.
    glfwPollEvents();
    // Swap buffers.
    glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Section 4: Modify below to add your key callbacks.
	 */

	 // Check for a key press.
	if (action == GLFW_PRESS)
	{
		// Uppercase key presses (shift held down + key press)
		if (mods == GLFW_MOD_SHIFT) {
            switch (key) {
                default:
                    break;
            }
		}
        else{
		// Deals with lowercase key presses
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    // Close the window. This causes the program to also terminate.
                    glfwSetWindowShouldClose(window, GL_TRUE);
                    break;
                case GLFW_KEY_N:
                    camera = !camera;
                    break;
                case GLFW_KEY_M:
                    move = !move;
                    break;
                default:
                    break;
            }
        }
	}
     
}

void Window::cursorCallback(GLFWwindow *window, double xpos, double ypos){
    set_cursor_position(xpos, ypos);
    calculate_rotation(camera);
}


void Window::buttonCallback(GLFWwindow* window, int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        press(true);
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        press(false);
    }
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    if(!camera){
        Army->set_scale(yoffset);
    }
}

// load image file into texture object
void Window::loadTexture()
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    cubemapTexture = textureID;
}

void Window::set_cursor_position(float xpos, float ypos){
    current_x = std::min(std::max(0.0f,xpos), float(width));
    current_y = std::min(std::max(0.0f,ypos), float(height));
}

void Window::calculate_rotation(bool camera){
    if(pressed && (current_x != start_x && current_y != start_y)){
        
        glm::vec3 v;
        float d;
        v.x = (2.0f*start_x - width) / width;
        v.y = (height - 2.0f*start_y) / height;
        v.z = 0.0f;
        d = glm::length(v);
        d = (d < 1.0f) ? d : 1.0f;
        v.z = sqrtf(1.001f - d*d);
        v = glm::normalize(v);
        
        glm::vec3 w;
        w.x = (2.0f*current_x - width) / width;
        w.y = (height - 2.0f*current_y) / height;
        w.z = 0.0f;
        d = glm::length(w);
        d = (d < 1.0f) ? d : 1.0f;
        w.z = sqrtf(1.001f - d*d);
        w = glm::normalize(w);
        
        float deg = acos(glm::dot(v, w) / (glm::length(v) * glm::length(w)));
        
        glm::vec3 trackball = glm::cross(v, w);
        
        if(glm::length(trackball) > 0.001f && abs(deg) > 0.001f){
            
            if(camera){
                view = glm::translate(glm::mat4(1.0f), -eye) * view;
                view = glm::rotate(glm::mat4(1.0f), glm::radians(100.0f * deg), trackball) * view;
                view = glm::translate(glm::mat4(1.0f), eye) * view;
            }
            else{
                Root->set_rotate(trackball, deg);                
            }
        }
        start_x = current_x;
        start_y = current_y;
    }
}

void Window::press(bool pressing){
    if(pressing){
        pressed = true;
        start_x = std::min(std::max(0.0f,current_x), float(width));
        start_y = std::min(std::max(0.0f,current_y), float(height));
    }
    else{
        pressed = false;
    }
}

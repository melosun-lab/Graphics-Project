#include "Window.h"

/* 
 * Declare your variables below. Unnamed namespace is used here to avoid 
 * declaring global or static variables.
 */
namespace
{
	int width, height;
	std::string windowTitle("GLFW Starter Project");

	// Boolean switches
	bool glRasterize = true;
    bool normal_coloring = true;
    bool light_control = false;
    bool toggle_on = true;
    bool point_light = false;
    bool show_light = false;

    float deg = 0.0;

	PointCloud* bearPoints;
    PointCloud* bunnyPoints;
    PointCloud* dragonPoints;
    PointCloud* lightPoints;
	PointCloud* currentObj;  // The object currently displaying.
	RasterizerQuad* quad;  // Object textured with your rasterization results

	glm::vec3 eye(0, 0, 20); // Camera position.
	glm::vec3 center(0, 0, 0); // The point we are looking at.
	glm::vec3 up(0, 1, 0); // The up direction of the camera.
	float fovy = 60;
	float near = 1;
	float far = 1000;
	glm::mat4 view = glm::lookAt(eye, center, up); // View matrix, defined by eye, center and up.
	glm::mat4 projection; // Projection matrix.

	GLuint program; // The shader program id.
	GLuint projectionLoc; // Location of projection in shader.
	GLuint viewLoc; // Location of view in shader.
	GLuint modelLoc; // Location of model in shader.
	GLuint colorLoc; // Location of color in shader.
    GLuint ambientLoc;
    GLuint diffuseLoc;
    GLuint specularLoc;
    GLuint shininessLoc;
    GLuint boolLoc;
    GLuint direactLightColorLoc;
    GLuint direactLightDirLoc;
    GLuint pointLightColorLoc;
    GLuint pointLightPosLoc;
    GLuint darkBoolLoc;

	GLuint programQuad;
};

bool Window::initializeProgram()
{
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	// This shader program is for displaying your rasterizer results
	// DO NOT MODIFY THESE SHADER PROGRAMS
	programQuad = LoadShaders("shaders/RasterizerQuad.vert", "shaders/RasterizerQuad.frag");

	// Check the shader programs.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}
	if (!programQuad)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Activate the shader program.
	glUseProgram(program);
	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	colorLoc = glGetUniformLocation(program, "color");
    ambientLoc = glGetUniformLocation(program, "ambient");
    diffuseLoc = glGetUniformLocation(program, "diffuse");
    specularLoc = glGetUniformLocation(program, "specular");
    shininessLoc = glGetUniformLocation(program, "shininess");
    boolLoc = glGetUniformLocation(program, "normal_coloring");
    direactLightColorLoc = glGetUniformLocation(program, "direct_light_color");
    direactLightDirLoc = glGetUniformLocation(program, "direct_light_dir");
    pointLightColorLoc = glGetUniformLocation(program, "point_light_color");
    pointLightPosLoc = glGetUniformLocation(program, "point_light_pos");
    darkBoolLoc = glGetUniformLocation(program, "dark");
    return true;
}

bool Window::initializeObjects()
{
    bunnyPoints = new PointCloud("bunny.obj", 1);
    dragonPoints = new PointCloud("dragon.obj", 1);
    bearPoints = new PointCloud("bear.obj", 1);
    lightPoints = new PointCloud("sphere.obj", 1);
    currentObj = bunnyPoints;
    bunnyPoints->set_size(width, height);
    dragonPoints->set_size(width, height);
    bearPoints->set_size(width, height);
    lightPoints->set_size(width, height);
    lightPoints->scaleUp(false);
    lightPoints->scaleUp(false);
    lightPoints->transform("up");
    lightPoints->transform("left");

	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	//delete cube;
	//delete cubePoints;
    delete bunnyPoints;
    delete dragonPoints;
    delete bearPoints;
    delete lightPoints;
	delete quad;

	// Delete the shader programs.
	glDeleteProgram(program);
	glDeleteProgram(programQuad);
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
	quad = new RasterizerQuad(width, height);

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

	// Resize our CPU rasterizer's pixel buffer and zbuffer
	quad->updateBufSiz(width, height);

	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	projection = glm::perspective(glm::radians(fovy),
		(float)width / (float)height, near, far);
    if(bunnyPoints){bunnyPoints->set_size(width, height);}
    if(dragonPoints){dragonPoints->set_size(width, height);}
    if(bearPoints){bearPoints->set_size(width, height);}
    if(lightPoints){lightPoints->set_size(width, height);}
}

void Window::idleCallback()
{
	// Perform any updates as necessary. 
	currentObj->update();
}

void Window::displayCallback(GLFWwindow* window)
{
	// Switch between OpenGL rasterizer and your rasterizer
	if (glRasterize) {
		// Switch back to using OpenGL's rasterizer
		glUseProgram(program);
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
        glm::mat4 model = currentObj->getModel();
        glm::vec3 color = currentObj->getColor();
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        
        if(normal_coloring){glUniform1i(boolLoc, 1);}
        else{glUniform1i(boolLoc, 0);}
        
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess = 0.0;
        
        if(currentObj == bunnyPoints){ //material 1
            color = glm::vec3(0.837255, 0.560784, 0.660784);
            ambient = glm::vec3(0.1, 0.1, 0.1);
            diffuse = glm::vec3(0.0, 0.0, 0.0);
            specular = glm::vec3(1.0, 1.0, 1.0);
            shininess = 1.0;
            
        }
        
        else if(currentObj == dragonPoints){ //material 2
            color = glm::vec3(0.137255, 0.556863, 0.419608);
            ambient = glm::vec3(0.1, 0.1, 0.1);
            diffuse = glm::vec3(1.0, 1.0, 1.0);
            specular = glm::vec3(0.0, 0.0, 0.0);
            shininess = 0.5;
        }
        
        else{ //material 3
            color = glm::vec3(0.71, 0.65, 0.26);
            ambient = glm::vec3(0.1, 0.1, 0.1);
            diffuse = glm::vec3(1.0, 1.0, 1.0);
            specular = glm::vec3(1.0, 1.0, 1.0);
            shininess = 0.5;
        }
        
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
        glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
        glUniform3fv(specularLoc, 1, glm::value_ptr(specular));
        glUniform1f(shininessLoc, shininess);
        
        //directional light
        deg += 0.05;
        
        glm::vec3 directLightColor = glm::vec3(0.6, 0.3, 0.3);
        if(!toggle_on){directLightColor = glm::vec3(0.0, 0.0, 0.0);}
        glm::vec3 directLightDir = glm::vec3(2.0, 2.0, 2.0);
        directLightDir = glm::rotate(glm::mat4(1.0f), glm::radians(deg), glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(directLightDir, 0.0);
        glm::vec3 pointLightColor = glm::vec3(0.9, 1.0, 0.1);
        if(!point_light){pointLightColor = glm::vec3(0.0, 0.0, 0.0);}
        glm::vec3 pointLightPos = lightPoints->get_pos();
        
        glUniform3fv(direactLightColorLoc, 1, glm::value_ptr(directLightColor));
        glUniform3fv(direactLightDirLoc, 1, glm::value_ptr(-directLightDir));
        glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));
        glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPos));
        
        
        if(deg == 360.0){deg = 0;}
        
        if (!show_light && !toggle_on){
            glUniform1i(darkBoolLoc, 1);
        }
        else{
            glUniform1i(darkBoolLoc, 0);
        }
        
        currentObj->draw();
        
        
        
        model = lightPoints->getModel();
        color = lightPoints->getColor();
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        
        color = glm::vec3(1.0, 1.0, 1.0);
        ambient = pointLightColor;
        diffuse = glm::vec3(0.0, 0.0, 0.0);
        specular = glm::vec3(0.0, 0.0, 0.0);
        shininess = 0.0;
        
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        glUniform3fv(ambientLoc, 1, glm::value_ptr(ambient));
        glUniform3fv(diffuseLoc, 1, glm::value_ptr(diffuse));
        glUniform3fv(specularLoc, 1, glm::value_ptr(specular));
        glUniform1f(shininessLoc, shininess);
        
        
        
        if(!normal_coloring){if(show_light)lightPoints->draw();}
		
		// Gets events, including input such as keyboard and mouse or window resizing.
		glfwPollEvents();
		// Swap buffers.
		glfwSwapBuffers(window);
	}
	else {
		// Uncomment when you want to see your rasterizer results
		glUseProgram(programQuad);
		
		/*
		 * TODO: Section 5: Fill in clearBuffers() and rasterize() with your code. You should make sure to
		 * pass in the correct M, V, P, and D matrices to rasterize()
		 */
        
		quad->clearBuffers();
		glm::mat4 M = glm::mat4(1.0f);
		glm::mat4 V = glm::mat4(1.0f);
		glm::mat4 P = glm::mat4(1.0f);
		glm::mat4 D = glm::mat4(0.0f);
        M = currentObj->getModel();
        V = view;
        P = projection;
        
        D[0][0] = width/2;
        D[3][0] = width/2;
        D[1][1] = height/2;
        D[3][1] = height/2;
        D[2][2] = 1.0f/2.0f;
        D[3][2] = 1.0f/2.0f;
        D[3][3] = 1.0f;
        
     
        
		// CPU based rasterization. Fills in the pixel buffer and displays result. 
		// Replace cubePoints with the PointCloud of your choice
        if(currentObj == dragonPoints){quad->rasterize(M, V, P, D, *dragonPoints);}
        else{quad->rasterize(M, V, P, D, *bunnyPoints);}

		glfwPollEvents();
		glfwSwapBuffers(window);
		
	}
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
            case GLFW_KEY_S:
                currentObj->scaleUp(true);
                break;
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
                case GLFW_KEY_F1:
                    // Set currentObj to dragon
                    currentObj = bunnyPoints;
                    break;
                case GLFW_KEY_F2:
                    // Set currentObj to bunny
                    currentObj = dragonPoints;
                    break;
                case GLFW_KEY_F3:
                // Set currentObj to bunny
                    currentObj = bearPoints;
                    break;
                case GLFW_KEY_S:
                    currentObj->scaleUp(false);
                    break;
                case GLFW_KEY_N:
                    if(normal_coloring){normal_coloring = false;}
                    else{normal_coloring = true;}
                    break;
                case GLFW_KEY_1:
                    point_light = false;
                    light_control = false;
                    show_light = false;
                    if(!toggle_on){toggle_on = true;}
                    break;
                case GLFW_KEY_2:
                    point_light = true;
                    show_light = true;
                    if(!toggle_on){toggle_on = true;}
                    light_control = true;
                    break;
                case GLFW_KEY_4:
                    toggle_on = !toggle_on;
                    break;
                case GLFW_KEY_0:
                    light_control = !light_control;
                break;
                default:
                    break;
            }
        }
	}
     
}

void Window::cursorCallback(GLFWwindow *window, double xpos, double ypos){
    if(light_control){
        lightPoints->set_cursor_position(xpos, ypos);
        lightPoints->calculate_light_rotation();
        lightPoints->calculate_translation();
    }
    else{
        currentObj->set_cursor_position(xpos, ypos);
        currentObj->calculate_rotation();
        currentObj->calculate_translation();
    }
}


void Window::buttonCallback(GLFWwindow* window, int button, int action, int mods){
    if(light_control){
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            lightPoints->press(true);
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            lightPoints->press(false);
        }
    
//        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
//            lightPoints->press_right(true);
//        }
//        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
//            lightPoints->press_right(false);
//        }
    }
    else{
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            currentObj->press(true);
        }
        if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            currentObj->press(false);
        }
        
        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
            currentObj->press_right(true);
        }
        if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
            currentObj->press_right(false);
        }
    }
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    if(!light_control){
        currentObj->move_z(yoffset);
    }
}

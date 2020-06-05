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
	Terrain* ter;
	Cube* cubePoints;
    Floor* skyfloor;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	bool firstMouse = true;
	bool firstTimeB = true;
	bool firstTimeM = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = width / 2.0f;
	float lastY = height / 2.0f;

	Cube* cube;
	Object* currentObj; // The object currently displaying.
	glm::vec3 eye(50, 20, 120);
	glm::vec3 center(0, 0, 0); // The point we are looking at.
	float fovy = 60;
	float near = 1;
	float far = 1000;


	glm::vec3 cameraPos = glm::vec3(0.0, 2.0, 20.0);
	glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraFront);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);

	glm::mat4 projection; // Projection matrix.

    GLuint screen;
    GLuint skybox;
	GLuint program; // The shader program id.
	GLuint projectionLoc; // Location of projection in shader.
	GLuint viewLoc; // Location of view in shader.
	GLuint modelLoc; // Location of model in shader.
	GLuint colorLoc; // Location of color in shader.
	GLuint cubeLoc;
	GLuint modeLoc;

	Transform* building1;
	Transform* building2;
	Transform* building3;
	Transform* world;
	Geometry* cubeL;
	Geometry* cubeM;
	Geometry* cubeS;
	Geometry* coneS;
	Geometry* domeS;
	Transform* cubeLT;
	Transform* cubeMT;
	Transform* cubeST;
	Transform* coneST;
	Transform* domeST;
	Geometry* cubeLL;
	Geometry* cubeMM;
	Transform* cubeLLT;
	Transform* cubeMMT;
	Geometry* cubeLLL;
	Geometry* cubeMMM;
	Transform* cubeLLLT;
	Transform* cubeMMMT;
	Map* map;

    Geometry* stadium;
	Geometry* road;
    Gui* gui;
	std::vector<Transform*> buildingBlock;
	std::vector<Transform*> roadBlock;

	bool shift = false;

	glm::vec3 prevPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 currPos = glm::vec3(0.0f, 0.0f, 0.0f);
	GLfloat angle = 0;
	glm::vec3 rotateAxis = glm::vec3(0.0f, 0.0f, 0.0f);

};

bool Window::initializeProgram()
{
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}
    
    skybox = LoadShaders("shaders/skybox.vert", "shaders/skybox.frag");

    // Check the shader program.
    if (!skybox)
    {
        std::cerr << "Failed to initialize skybox program" << std::endl;
        return false;
    }
    
    screen = LoadShaders("shaders/motion.vert", "shaders/motion.frag");

    // Check the shader program.
    if (!screen)
    {
        std::cerr << "Failed to initialize screen program" << std::endl;
        return false;
    }
    
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    unsigned int depthMap;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
       glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
       glDrawBuffer(GL_NONE);
       glReadBuffer(GL_NONE);
       glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Activate the shader program.
	glUseProgram(program);

	// Get the locations of uniform variables.
	projectionLoc = glGetUniformLocation(program, "projection");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	colorLoc = glGetUniformLocation(program, "color");

	return true;
}

bool Window::initializeObjects()
{
	// Create a cube of size 5.
	cube = new Cube(1000.0f);
	currentObj = cube;

	road = new Geometry("road.obj", 2, program, 4, 1.0f);
    stadium = new Geometry("stadium.obj", 1, program, 1, 1.0f);

	map = new Map();
	map->printMap();
    
    gui = new Gui(screen, glm::vec2(0,0), glm::vec2(width/2, height/2));

	glm::mat4 worldMat{ 1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1 };
	world = new Transform(worldMat, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);

	building1 = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	building2 = new Transform(glm::mat4{ 1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	building3 = new Transform(glm::mat4{ 1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeLT = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeL = new Geometry("cube.obj", 4, program, 1, 6.0f);
	cubeMT = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,5,0,1 },
		false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeM = new Geometry("cube.obj", 3.5, program, 1, 6.0f);
	cubeLLT = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeLL = new Geometry("cube.obj", 4, program, 2, 6.0f);
	cubeMMT = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,5,0,1 },
		false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeMM = new Geometry("cube.obj", 3.5, program, 2, 6);
	cubeLLLT = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeLLL = new Geometry("cube.obj", 4, program, 3, 6);
	cubeMMMT = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,5,0,1 },
		false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeMMM = new Geometry("cube.obj", 3.5, program, 3, 6.0f);

	building1->addChild(cubeLT);
	building1->addChild(cubeMT);
	building2->addChild(cubeLLT);
	building2->addChild(cubeMMT);
	building3->addChild(cubeLLLT);
	building3->addChild(cubeMMMT);
	cubeLT->addChild(cubeL);
	cubeMT->addChild(cubeM);
	cubeLLT->addChild(cubeLL);
	cubeMMT->addChild(cubeMM);
	cubeLLLT->addChild(cubeLLL);
	cubeMMMT->addChild(cubeMMM);

	genBuilding();
	// start building map

	buildCity();

	ter = new Terrain(8, 50, 0, 0, 0, 0, 50);
//	skyfloor = new Cube(100.0f);
//	skyfloor->setScale(11.12, 0.2, 11.12);
    skyfloor = new Floor(100.0f);
    skyfloor->setScale(11.12, 0.2, 11.12);
	return true;
}

void Window::cleanUp()
{
	// Deallcoate the objects.
	delete world;
	delete map;
	delete ter;
	delete cubeL;
	delete cubeM;
	delete cubeS;
	delete cubeLT;
	delete cubeMT;
	delete cubeST;
	delete cube;
	delete road;
    delete gui;

	delete building1;
	delete building2;
	delete building3;
	delete coneS;
	delete domeS;
	delete coneST;
	delete domeST;
	delete cubeLL;
	delete cubeMM;
	delete cubeLLT;
	delete cubeMMT;
	delete cubeLLL;
	delete cubeMMM;
	delete cubeLLLT;
	delete cubeMMMT;

	for (int i = 0; i < buildingBlock.size(); i++) {
		delete buildingBlock[i];
	}
    
    for (int i = 0; i < roadBlock.size(); i++) {
        delete roadBlock[i];
    }

	// Delete the shader program.
	glDeleteProgram(program);
    glDeleteProgram(screen);
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
	//currentObj->update();
}

void Window::displayCallback(GLFWwindow* window)
{
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(skybox);
	// Specify the values of the uniform variables we are going to use.
	glm::mat4 model = currentObj->getModel();
	glm::vec3 color = currentObj->getColor();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    currentObj->draw();
    
    glUseProgram(program);
	GLuint backgroundLoc = glGetUniformLocation(program, "background");
	//glUniform1f(backgroundLoc, 1.0f);
	glUniform1f(backgroundLoc, 0.0f);
	world->draw(glm::mat4(1.0f));

	//glUseProgram(program);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	processInput(window);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	model = skyfloor->getModel();
    model = glm::translate(model, glm::vec3(0.0, -0.8, 0.0));
	//std::cout << glm::to_string(model);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniform1f(backgroundLoc, 0.0f);
	skyfloor->draw();

	model = ter->getModel();
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.08f, 0.08f, 0.08f));
    bool continueLoop = true;
	// TODO modify it so that it enters the scene graph
	for (int i = 0; i < map->charMap.size() && continueLoop; i++) {
		for (int j = 0; j < map->charMap[i].size(); j++) {
			if (map->charMap[i][j] == 'T') {
				model = glm::translate(model, glm::vec3(-130 + i * 26, 0.6, -80+j*26));
                continueLoop = false;
				break;
			}
		}
	}

    glUniform1f(backgroundLoc, 0.0f);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	ter->draw();
    
    continueLoop = true;
    glm::mat4 transform;
    for (int i = 0; i < map->charMap.size() && continueLoop; i++) {
        for (int j = 0; j < map->charMap[i].size(); j++) {
            if (map->charMap[i][j] == 'A') {
                transform = glm::mat4{ 1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -8+i*2,0.6,-8+j*2,1
                };
                continueLoop = false;
                break;
            }
        }
    }
    stadium->draw(glm::mat4{transform});
    
    glUseProgram(screen);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gui->draw();
    glDisable(GL_BLEND);

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const float cameraSpeed = 0.005f;
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		// Uppercase key presses (shift held down + key press)
		if (mods == GLFW_MOD_SHIFT) {
			switch (key) {
			default:
				shift = true;
				break;
			}
		}
		else {
			// Deals with lowercase key presses
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				// Close the window. This causes the program to also terminate.
				glfwSetWindowShouldClose(window, GL_TRUE);
				break;
			
			case GLFW_KEY_N:
                delete ter;
                    std::cout <<"Generating new terrain." << std::endl;
				ter = new Terrain(8, 50, 0, 0, 0, 0, 50);
				break;
			case GLFW_KEY_M:
				delete map;
				std::cout << "Generating new city map." << std::endl;
				map = new Map();
				map->printMap();
				buildCity();
				break;
			case GLFW_KEY_B:
				std::cout << "Generating new building." << std::endl;
				genBuilding();
				break;
			default:
				shift = false;
				break;
			}
		}
	}
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	if (xpos >= width - 1 || xpos <= 1.0) {
		glfwSetCursorPos(window, width / 2.0, height / 2.0);
		lastX = width / 2.0;
		lastY = height / 2.0;
		xoffset = 0.0;
	}

	float sensitivity = 0.5f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
};

void Window::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (shift) {
			cameraPos += cameraSpeed * up;
		}
		else {
			cameraPos -= cameraSpeed * up;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void Window::buildCity() {

	if (firstTimeM) {
		firstTimeM = false;
	} else {
		for (int i = 0; i < buildingBlock.size(); i++) {
            buildingBlock[i]->list.clear();
			//delete buildingBlock[i];
		}
		for (int i = 0; i < roadBlock.size(); i++) {
            roadBlock[i]->list.clear();
			//delete roadBlock[i];
		}
		buildingBlock.clear();
		roadBlock.clear();
	}

	for (int i = 0; i < map->charMap.size(); i++) {
		for (int j = 0; j < map->charMap[i].size(); j++) {
			if (map->charMap[i][j] == 'B') {
				buildingBlock.push_back(new Transform(glm::mat4{
					0.18,0,0,0,
					0,0.18,0,0,
					0,0,0.18,0,
					-9 + j * 2,0.6,-9 + i * 2,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f));
				int random = rand() % 3;
				if (random == 0) {
					buildingBlock[buildingBlock.size() - 1]->addChild(building1);
				}
				else if (random == 1) {
					buildingBlock[buildingBlock.size() - 1]->addChild(building2);
				}
				else {
					buildingBlock[buildingBlock.size() - 1]->addChild(building3);
				}
			}
			else if (map->charMap[i][j] == '|') {
				glm::mat4 tmp = glm::mat4{
					0.4,0,0,0,
					0,0.4,0,0,
					0,0,0.4,0,
					-9 + j * 2,0.1,-9 + i * 2,1 };
				roadBlock.push_back(new Transform(tmp, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f));
				roadBlock[roadBlock.size() - 1]->addChild(road);
			}
			else if (map->charMap[i][j] == '-' || map->charMap[i][j] == '+') {
				glm::mat4 tmp = glm::mat4{
					0.4,0,0,0,
					0,0.4,0,0,
					0,0,0.4,0,
					-9 + j * 2,0.1,-9 + i * 2,1 };
				//tmp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * tmp;
				tmp = glm::rotate(tmp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				roadBlock.push_back(new Transform(tmp, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f));
				roadBlock[roadBlock.size() - 1]->addChild(road);
			}
		}
	}

	for (int i = 0; i < buildingBlock.size(); i++) {
		world->addChild(buildingBlock[i]);
	}

	for (int i = 0; i < roadBlock.size(); i++) {
		world->addChild(roadBlock[i]);
	}
}

void Window::genBuilding() {
	if (firstTimeB) {
		firstTimeB = false;
	} else {
		(building1->list).erase((building1->list).begin() + 2);
		(building2->list).erase((building2->list).begin() + 2);
		(building3->list).erase((building3->list).begin() + 2);
		delete cubeS;
		delete cubeST;
		delete coneS;
		delete coneST;
		delete domeS;
		delete domeST;
	}

	// 1 for building 1, 2 for building 2, 3 for building3
	srand((unsigned)time(NULL));
	int tmprand = rand() % 2 + 2;
	cubeST = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,9,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	cubeS = new Geometry("cube.obj", tmprand, program, 1, 6.0f);

	domeST = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,10,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	tmprand = rand() % 3 + 1;
	domeS = new Geometry("dome.obj", tmprand, program, 2, 6.0f);

	coneST = new Transform(glm::mat4{ 1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,10,0,1 }, false, 0.0f, true, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.5f);
	srand((unsigned)time(NULL) + 10);
	tmprand = rand() % 3 + 1;
	coneS = new Geometry("cone.obj", tmprand, program, 3, 6.0f);

	building1->addChild(cubeST);
	cubeST->addChild(cubeS);
	building2->addChild(domeST);
	domeST->addChild(domeS);
	building3->addChild(coneST);
	coneST->addChild(coneS);
}

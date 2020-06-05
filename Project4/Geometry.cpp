#include "Geometry.h"

Geometry::Geometry(std::string filename, float scale, GLuint program, int text, int textS) {
    //name = 'G';
	prog = program;
	initialization(filename, scale);
	type = text;
	//std::cout << type;
	loadTexture();
	textSample = textS;
};

Geometry::~Geometry() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vbot);
    glDeleteBuffers(1, &vbon);
    glDeleteVertexArrays(1, &vao);
}


void Geometry::setMatrix(glm::mat4 C) {
	M = C;
	//std::cout << glm::to_string(C);
	glUseProgram(prog);
	GLuint modelLoc = glGetUniformLocation(prog, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));
};


void Geometry::initialization(std::string filename, float scale) {
	FILE* fp;
	float x, y, z;
	float xn, yn, zn, xt, yt;
	float maxx, minx, maxy, miny, maxz, minz;
	int fx, fy, fz, nx, ny, nz, fe, ne, tx, ty, tz;
	int c1, c2;

	fp = fopen(strcpy(new char[filename.length() + 1], filename.c_str()), "rb");

	if (fp == NULL) { std::cerr << "error loading file" << std::endl; exit(-1); }

	c1 = fgetc(fp);
	c2 = c1; // stores the value of the previous character 

	maxx = maxy = maxz = std::numeric_limits<float>::min();
	minx = miny = minz = std::numeric_limits<float>::max();

	while (c1 != EOF) {
		if (c2 == 'v' && c1 == ' ') {
			fscanf(fp, "%f %f %f", &x, &y, &z);
			points.push_back(glm::vec3(x, y, z));
			if (maxx < x) {
				maxx = x;
			}
			if (minx > x) {
				minx = x;
			}
			if (maxy < y) {
				maxy = y;
			}
			if (miny > y) {
				miny = y;
			}
			if (maxz < z) {
				maxz = z;
			}
			if (minz > z) {
				minz = z;
			}
		}
		if (c2 == 'v' && c1 == 'n') {
			fscanf(fp, "%f %f %f", &xn, &yn, &zn);
			npoints.push_back(glm::vec3(xn, yn, zn));
		}
		if (c2 == 'v' && c1 == 't') {
			fscanf(fp, "%f %f", &xt, &yt);
			textures.push_back(glm::vec3(xt * 2.0f, yt * 2.0f, 0));
			//textures.push_back(glm::vec2(xt * textSample, yt * textSample));
		}
		if (c2 == 'f' && c1 == ' ') {
			fscanf(fp, "%i%*c%i%*c%i %i%*c%i%*c%i %i%*c%i%*c%i", &fx, &tx, &nx, &fy, &ty, &ny, &fz, &tz, &nz);
			//std::cout << fx << std::endl;
			verticesI.push_back(fx - 1);
			verticesI.push_back(fy - 1);
			verticesI.push_back(fz - 1);
			normalsI.push_back(nx - 1);
			normalsI.push_back(ny - 1);
			normalsI.push_back(nz - 1);
			texturesI.push_back(tx - 1);
			texturesI.push_back(ty - 1);
			texturesI.push_back(tz - 1);
		}
		c2 = c1;
		c1 = fgetc(fp);
	}

	// verticesI, normalsI have the same size, iterate through them

	fclose(fp);
	std::vector<glm::vec3> mypoints;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tmptex;

	// rearrange normal
	for (int i = 0; i < verticesI.size(); i++) {
		mypoints.push_back(points[verticesI[i]]);
		normals.push_back(npoints[normalsI[i]]);
		tmptex.push_back(textures[texturesI[i]]);
		faces.push_back(i);
	}

	points = mypoints;
	npoints = normals;
	textures = tmptex;

	float avgx = 0.5f * (maxx + minx);
	float avgy = 0.5f * (maxy + miny);
	float avgz = 0.5f * (maxz + minz);

	for (int i = 0; i < points.size(); i++) {
		points[i] = glm::vec3(scale * (points[i][0] - avgx), scale * (points[i][1] - avgy), scale * (points[i][2] - avgz));
	}

	 // Set the model matrix to an identity matrix. 
	model = glm::mat4(1.0f);
	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vbot);
	glGenBuffers(1, &vbon);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	// This tells OpenGL which data it should be paying attention to
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbot);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* textures.size(),
		textures.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbon);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* npoints.size(),
		npoints.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), faces.data(), GL_STATIC_DRAW);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);
};

void Geometry::draw(glm::mat4 C) {
	setMatrix(C);
	glUseProgram(prog);
	glBindVertexArray(vao);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawElements(GL_TRIANGLES, faces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
};

void Geometry::update(){
}

unsigned char* Geometry::loadPPM(const char* filename, int& width, int& height)
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

void Geometry::loadTexture() {
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texture);

	unsigned char* tdata;  // texture pixel data
	int twidth, theight;   // texture width/height [pixels]

	
	if (type == 1) {
		tdata = loadPPM("window1.ppm", twidth, theight);
	}
	else if (type == 2) {
		tdata = loadPPM("window2.ppm", twidth, theight);
	}
	else if (type == 3) {
		tdata = loadPPM("window3.ppm", twidth, theight);
	}
    else if (type == 4) {
		tdata = loadPPM("roadtext.ppm", twidth, theight);
	}
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

#include "Terrain.h"

Terrain::Terrain(int n, int len, float corner1, float corner2, float corner3, float corner4, float reach) {
    length = len;
    size = pow(2, n) + 1;
    reduce = reach;
    loadTexture();

    for (unsigned int i = 0; i < size; i++) {
        std::vector<float> temp;
        for (unsigned int j = 0; j < size; j++) {
            temp.push_back(0.0);
        }
        z.push_back(temp);
    }

    for (unsigned int i = 0; i < size; i++) {
        std::vector<glm::vec3> temp;
        for (unsigned int j = 0; j < size; j++) {
            temp.push_back(glm::vec3(0.0));
        }
        table.push_back(temp);
    }

    z[0][0] = corner1;
    z[0][size - 1] = corner2;
    z[size - 1][0] = corner3;
    z[size - 1][size - 1] = corner4;

    BFS(0, size - 1, 0, size - 1, reduce);

    inter = float(length) / float(size);

    float tx = 0.0;
    float ty = 0.0;

    for (unsigned int i = 0; i < size; i++) {
        ty = 0.0;
        for (unsigned int j = 0; j < size; j++) {
            points.push_back(glm::vec3(0.0 + j * inter, z[i][j], 0.0 - i * inter));
            table[i][j] = glm::vec3(0.0 + j * inter, z[i][j], 0.0 - i * inter);
            texcoord.push_back(glm::vec3(tx, ty, 0));
            ty += 0.01;
        }
        tx += 0.01;
    }

    for (unsigned int i = 0; i < size; i++) {
        for (unsigned int j = 0; j < size; j++) {
            if (i == 0) {
                if (j == 0) { //left-up cornor
                    glm::vec3 hori = table[i][j + 1] - table[i][j];
                    glm::vec3 vert = table[i + 1][j] - table[i][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
                else if (j == size - 1) { //right-up cornor
                    glm::vec3 hori = table[i][j] - table[i][j - 1];
                    glm::vec3 vert = table[i + 1][j] - table[i][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
                else { //up-edge
                    glm::vec3 hori = table[i][j + 1] - table[i][j - 1];
                    glm::vec3 vert = table[i + 1][j] - table[i][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
            }
            else if (i == size - 1) {
                if (j == 0) { //left-down cornor
                    glm::vec3 hori = table[i][j + 1] - table[i][j];
                    glm::vec3 vert = table[i][j] - table[i - 1][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
                else if (j == size - 1) { //right-down cornor
                    glm::vec3 hori = table[i][j] - table[i][j - 1];
                    glm::vec3 vert = table[i][j] - table[i - 1][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
                else { //down-edge
                    glm::vec3 hori = table[i][j + 1] - table[i][j - 1];
                    glm::vec3 vert = table[i][j] - table[i - 1][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
            }
            else {
                if (j == 0) { //left-edge
                    glm::vec3 hori = table[i][j + 1] - table[i][j];
                    glm::vec3 vert = table[i + 1][j] - table[i - 1][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
                else if (j == size - 1) { //right-edge
                    glm::vec3 hori = table[i][j] - table[i][j - 1];
                    glm::vec3 vert = table[i + 1][j] - table[i - 1][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
                else { //internal
                    glm::vec3 hori = table[i][j + 1] - table[i][j - 1];
                    glm::vec3 vert = table[i + 1][j] - table[i - 1][j];
                    glm::vec3 norm = glm::cross(hori, vert);
                    points_norm.push_back(norm);
                }
            }
        }
    }

    for (unsigned int i = 0; i < size - 1; i++) {
        for (unsigned int j = 0; j < size - 1; j++) {
            faces.push_back(j + i * size);
            faces.push_back(j + size + i * size);
            faces.push_back(j + 1 + i * size);
            faces.push_back(j + 1 + i * size);
            faces.push_back(j + size + i * size);
            faces.push_back(j + size + 1 + i * size);
        }
    }

    srand((unsigned)time(NULL));

    model = glm::mat4(1);
    color = glm::vec3(1, 0, 0);

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

Terrain::~Terrain() {
}

void Terrain::BFS(int startx, int endx, int starty, int endy, float reduce) {
    int leng = endx - startx;
    int full = endx - startx;
    int offset;
    while (leng > 1) {
        offset = leng;
        for (unsigned int i = 0; i <= full - offset; i += offset) {
            for (unsigned int j = 0; j <= full - offset; j += offset) {
                diamondSquare(i, i + offset, j, j + offset, reduce);
            }
        }
        leng /= 2;
        reduce /= 2.0f;
    }
}

void Terrain::diamondSquare(int startx, int endx, int starty, int endy, float reduce) {
    int leng = endx - startx;

    if (leng == 1) {
        return;
    }
    int midx = (startx + endx) / 2;
    int midy = (starty + endy) / 2;

    //mid point
    float avg = 0.0;
    avg += z[startx][starty] + z[startx][endy] + z[endx][starty] + z[endx][endy];
    avg /= 4.0;
    z[midx][midy] = std::max(0.0f, avg + random(reduce));

    //up point
    avg = 0.0;
    avg += z[startx][starty] + z[midx][midy] + z[startx][endy];
    if (midx - leng >= 0) {
        avg += z[midx - leng][midy];
        avg /= 4.0;
        z[startx][midy] = std::max(0.0f, avg + random(reduce));

    }
    else {
        avg /= 3.0;
        z[startx][midy] = std::max(0.0f, avg + random(reduce));
    }
    if (startx == 0)
        z[startx][midy] = 0.0;

    //down point
    avg = 0.0;
    avg += z[endx][starty] + z[endx][endy] + z[midx][midy];
    if (midx + leng < size) {
        avg += z[midx + leng][midy];
        avg /= 4.0;
        z[endx][midy] = std::max(0.0f, avg + random(reduce));
    }
    else {
        avg /= 3.0;
        z[endx][midy] = std::max(0.0f, avg + random(reduce));
    }
    if (endx == size - 1)
        z[endx][midy] = 0.0;

    //left point
    avg = 0.0;
    avg += z[startx][starty] + z[endx][starty] + z[midx][midy];
    if (midy - leng >= 0) {
        avg += z[midx][midy - leng];
        avg /= 4.0;
        z[midx][starty] = std::max(0.0f, avg + random(reduce));
    }
    else {
        avg /= 3.0;
        z[midx][starty] = std::max(0.0f, avg + random(reduce));
    }
    if (starty == 0)
        z[midx][starty] = 0.0;


    //right point
    avg = 0.0;
    avg += z[startx][endy] + z[endx][endy] + z[midx][midy];
    if (midy + leng < size) {
        avg += z[midx][midy + leng];
        avg /= 4.0;
        z[midx][endy] = std::max(0.0f, avg + random(reduce));
    }
    else {
        avg /= 3.0;
        z[midx][endy] = std::max(0.0f, avg + random(reduce));
    }
    if (endy == size - 1)
        z[midx][endy] = 0.0;
}

float Terrain::random(float range) {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX)* range - range / 2;
}

void Terrain::draw() {
    glActiveTexture(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_2D);
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, (int)faces.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Terrain::update() {

}

unsigned char* Terrain::loadPPM(const char* filename, int& width, int& height)
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

void Terrain::loadTexture() {
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);

    unsigned char* tdata;  // texture pixel data
    int twidth, theight;   // texture width/height [pixels]

    tdata = loadPPM("grass.ppm", twidth, theight);
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

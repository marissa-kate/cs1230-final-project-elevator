#include "cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    glm::vec3 normal) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
}

void Cube::makeFace(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 normal) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube'

    for (int j = 0; j < m_param1; ++j) {
        for (int i = 0; i < m_param1; ++i) {
            glm::vec3 scale_topLeft = topLeft + ((bottomRight - bottomLeft) * ((float)(i)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j)/m_param1));
            glm::vec3 scale_bottomLeft = topLeft + ((bottomRight - bottomLeft) * ((float)(i)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j + 1)/m_param1));
            glm::vec3 scale_topright = topLeft + ((bottomRight - bottomLeft) * ((float)(i + 1)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j)/m_param1));
            glm::vec3 scale_bottomright = topLeft + ((bottomRight - bottomLeft) * ((float)(i + 1)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j + 1)/m_param1));
            makeTile(scale_topLeft, scale_topright, scale_bottomLeft, scale_bottomright, normal);
        }
    }
}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:
    float s = 0.5f;
    float angle = 45.0f * 3.1415926f / 180.0f; // 45 degrees in radians
    float c = cos(angle);
    float s_sin = sin(angle);

    // Rotate a point around Y then X
    auto rotateCorner = [&](glm::vec3 v) {
        // Rotate around Y
        float x1 = c * v.x + s_sin * v.z;
        float z1 = -s_sin * v.x + c * v.z;
        float y1 = v.y;

        // Rotate around X
        float y2 = c * y1 - s_sin * z1;
        float z2 = s_sin * y1 + c * z1;
        float x2 = x1;

        return glm::vec3(x2, y2, z2);
    };


    makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(0.f, 0.f, 1.0f));

    // Uncomment these lines for Task 3:

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(0.f, 0.f, 1.0f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube

    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3(0.f, 1.f, 0.f));

    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(1.0f, 0.0f, 0.0f));

    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(0.f, 0.f, -1.0f));

    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(0.f, -1.f, 0.f));

    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec3(-1.f, 0.f, 0.f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

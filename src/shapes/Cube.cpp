#include "Cube.h"

void Cube::updateParams(int param1, int param2){
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}
glm::mat3 Cube::inertiaTensor(float m, glm::vec3 scale){
    float w = scale.x;
    float h = scale.y;
    float d = scale.z;
    return glm::mat3(
        (1.0f/12.0f) * m * (h*h + d*d), 0, 0,
        0, (1.0f/12.0f) * m * (w*w + d*d), 0,
        0, 0, (1.0f/12.0f) * m * (w*w + h*h)
        );
}
void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 n = glm::normalize(glm::cross(bottomRight-bottomLeft, topRight-bottomRight));

    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,n);
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,n);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,n);
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,n);
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,n);

}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()

    for (int i = 0; i < m_param1; i++){
        float v1 = (float)i / m_param1;
        float v2 = (float)(i + 1) / m_param1;
        //rows
        glm::vec3 leftTop    = glm::mix(topLeft, bottomLeft, v1);
        glm::vec3 leftBottom = glm::mix(topLeft, bottomLeft, v2);
        glm::vec3 rightTop   = glm::mix(topRight, bottomRight, v1);
        glm::vec3 rightBottom= glm::mix(topRight, bottomRight, v2);

        for(int j = 0; j < m_param1; j++){

            float u1 = (float)j / m_param1;
            float u2 = (float)(j + 1) / m_param1;

            glm::vec3 tl = glm::mix(leftTop, rightTop, u1);
            glm::vec3 tr = glm::mix(leftTop, rightTop, u2);
            glm::vec3 bl = glm::mix(leftBottom, rightBottom, u1);
            glm::vec3 br = glm::mix(leftBottom, rightBottom, u2);
            makeTile(tl, tr, bl, br);
        }
    }
}

void Cube::setVertexData() {

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));

    makeFace(glm::vec3(0.5f,  -0.5f, -0.5f),
             glm::vec3(0.5f,  -0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, 0.5f, 0.5f));

    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f));

    makeFace(glm::vec3(-0.5f,  -0.5f, -0.5f),
             glm::vec3(-0.5f,  -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f));
}

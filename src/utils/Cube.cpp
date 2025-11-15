#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal(glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));
    insertVec3(m_vertexData, glm::vec3(topLeft));
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, glm::vec3(bottomLeft));
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, glm::vec3(bottomRight));
    insertVec3(m_vertexData, normal);


    insertVec3(m_vertexData, glm::vec3(topLeft));
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, glm::vec3(bottomRight));
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, glm::vec3(topRight));
    insertVec3(m_vertexData, normal);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    float step = 1.f / m_param1;
    glm::vec3 right_delta = (topRight - topLeft) * step;
    glm::vec3 down_delta = (bottomLeft - topLeft) * step;
    for (int y = 0; y < m_param1; ++y){
        for (int x = 0; x < m_param1; ++x){
            glm::vec3 tile_TL = topLeft + ((float)x * right_delta) + ((float)y * down_delta);
            glm::vec3 tile_TR = tile_TL + right_delta;
            glm::vec3 tile_BL = tile_TL + down_delta;
            glm::vec3 tile_BR = tile_TL + right_delta + down_delta;
            makeTile(tile_TL, tile_TR, tile_BL, tile_BR);
        }
    }


}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

    // makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
    //          glm::vec3( 0.5f,  0.5f, 0.5f),
    //          glm::vec3(-0.5f, -0.5f, 0.5f),
    //          glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:


    // Task 4: Use the makeFace() function to make all 6 sides of the cube


    makeFace(glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    makeFace(glm::vec3( 0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    makeFace(glm::vec3( 0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));


    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));

    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3( 0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, 0.5f));


    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

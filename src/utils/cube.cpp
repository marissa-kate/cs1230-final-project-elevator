#include "cube.h"

cube::cube() {}

void cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void cube::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

void cube::makeCubeTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                          glm::vec3 topRight, glm::vec2 topRight_uv,
                          glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                          glm::vec3 bottomRight, glm::vec2 bottomRight_uv,
                          glm::vec3 normal) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    insertVec3(m_CubevertexData, topLeft);
    insertVec3(m_CubevertexData, normal);
    insertVec2(m_CubevertexData, topLeft_uv);
    insertVec3(m_CubevertexData, bottomLeft);
    insertVec3(m_CubevertexData, normal);
    insertVec2(m_CubevertexData, bottomLeft_uv);
    insertVec3(m_CubevertexData, bottomRight);
    insertVec3(m_CubevertexData, normal);
    insertVec2(m_CubevertexData, bottomRight_uv);

    insertVec3(m_CubevertexData, topRight);
    insertVec3(m_CubevertexData, normal);
    insertVec2(m_CubevertexData, topRight_uv);
    insertVec3(m_CubevertexData, topLeft);
    insertVec3(m_CubevertexData, normal);
    insertVec2(m_CubevertexData, topLeft_uv);
    insertVec3(m_CubevertexData, bottomRight);
    insertVec3(m_CubevertexData, normal);
    insertVec2(m_CubevertexData, bottomRight_uv);

}

void cube::makeFace(glm::vec3 topLeft, glm::vec2 top_leftuv,
                    glm::vec3 topRight, glm::vec2 top_rightuv,
                    glm::vec3 bottomLeft, glm::vec2 bottom_leftuv,
                    glm::vec3 bottomRight, glm::vec2 bottom_rightuv,
                    glm::vec3 normal) {
    for (int j = 0; j < m_param1; ++j) {
        for (int i = 0; i < m_param1; ++i) {
            glm::vec3 scale_topLeft = topLeft + ((bottomRight - bottomLeft) * ((float)(i)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j)/m_param1));
            glm::vec2 scale_topLeft_uv = top_leftuv + ((bottom_rightuv - bottom_leftuv) * (((float)(i))/m_param1)) - ((top_leftuv - bottom_leftuv) * (((float)(j))/m_param1));
            glm::vec3 scale_bottomLeft = topLeft + ((bottomRight - bottomLeft) * ((float)(i)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j + 1)/m_param1));
            glm::vec2 scale_bottomLeft_uv = top_leftuv + ((bottom_rightuv - bottom_leftuv) * (((float)(i))/m_param1)) - ((top_leftuv - bottom_leftuv) * (((float)(j + 1))/m_param1));
            glm::vec3 scale_topright = topLeft + ((bottomRight - bottomLeft) * ((float)(i + 1)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j)/m_param1));
            glm::vec2 scale_topRight_uv = top_leftuv + ((bottom_rightuv - bottom_leftuv) * (((float)(i + 1))/m_param1)) - ((top_leftuv - bottom_leftuv) * (((float)(j))/m_param1));
            glm::vec3 scale_bottomright = topLeft + ((bottomRight - bottomLeft) * ((float)(i + 1)/m_param1)) - ((topLeft - bottomLeft) * ((float)(j + 1)/m_param1));
            glm::vec2 scale_bottomRight_uv = top_leftuv + ((bottom_rightuv - bottom_leftuv) * (((float)(i + 1))/m_param1)) - ((top_leftuv - bottom_leftuv) * (((float)(j + 1))/m_param1));
            makeCubeTile(scale_topLeft, scale_topLeft_uv,
                         scale_topright, scale_topRight_uv,
                         scale_bottomLeft, scale_bottomLeft_uv,
                         scale_bottomright, scale_bottomRight_uv,
                         normal);
        }
    }
}

void cube::setCubeVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:
    float s = 0.5f;
    float angle = 45.0f * M_PI / 180.0f; // 45 degrees in radians
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


    makeCubeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
                 glm::vec2(0.0f, 1.0f),
                 glm::vec3( 0.5f,  0.5f, 0.5f),
                 glm::vec2(1.0f, 1.0f),
                 glm::vec3(-0.5f, -0.5f, 0.5f),
                 glm::vec2(0.0f, 0.0f),
                 glm::vec3( 0.5f, -0.5f, 0.5f),
                  glm::vec2(1.0f, 0.0f),
                 glm::vec3(0.f, 0.f, 1.0f));

    // Uncomment these lines for Task 3:

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec2(0.0f, 1.0f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec2(1.0f, 1.0f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec2(0.0f, 0.0f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec2(1.0f, 0.0f),
             glm::vec3(0.f, 0.f, 1.0f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube

    //top
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec2(0.0f, 1.0f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec2(1.0f, 1.0f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec2(0.0f, 0.0f),
             glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec2(1.0f, 0.0f),
             glm::vec3(0.f, 1.f, 0.f));

    //right
    makeFace(glm::vec3( 0.5f,  0.5f,  0.5f),
             glm::vec2(0.0f, 1.0f),
             glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec2(1.0f, 1.0f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec2(0.0f, 0.0f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec2(1.0f, 0.0f),
             glm::vec3(1.0f, 0.0f, 0.0f));

    //back
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec2(0.0f, 1.0f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec2(1.0f, 1.0f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec2(0.0f, 0.0f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec2(1.0f, 0.0f),
             glm::vec3(0.f, 0.f, -1.0f));

    //bottom
    makeFace(glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec2(0.0f, 1.0f),
             glm::vec3( 0.5f, -0.5f,  0.5f),
             glm::vec2(1.0f, 1.0f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec2(0.0f, 0.0f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec2(1.0f, 0.0f),
             glm::vec3(0.f, -1.f, 0.f));

    //left
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec2(0.0f, 1.0f),
             glm::vec3(-0.5f,  0.5f,  0.5f),
             glm::vec2(1.0f, 1.0f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec2(0.0f, 0.0f),
             glm::vec3(-0.5f, -0.5f,  0.5f),
             glm::vec2(1.0f, 0.0f),
             glm::vec3(-1.f, 0.f, 0.f));
}

std::vector<float> cube::generateCubeData(int phiTesselations, int thetaTesselations) {
    m_CubevertexData.clear();
    m_param1 = phiTesselations;
    m_param2 = thetaTesselations;
    setCubeVertexData();
    return m_CubevertexData;
}

#include "Cube.h"
#include <iostream>

void Cube::updateParams(int param1, int param2) {
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
                    glm::vec3 bottomRight,
                    int face) {

    glm::vec3 left = topLeft - bottomLeft;
    glm::vec3 bottom = bottomRight - bottomLeft;
    glm::vec3 normal = glm::normalize(glm::cross(bottom, left));


    //for each vertex, add vec3 position, vec3 normals, vec2 uv values, vec3 dp/du, and vec3 dp/dv

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, getUV(topLeft, face));
    insertVec3(m_vertexData, getPu(topLeft, face));
    insertVec3(m_vertexData, getPv(topLeft, face));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, getUV(bottomLeft, face));
    insertVec3(m_vertexData, getPu(bottomLeft, face));
    insertVec3(m_vertexData, getPv(bottomLeft, face));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, getUV(bottomRight, face));
    insertVec3(m_vertexData, getPu(bottomRight, face));
    insertVec3(m_vertexData, getPv(bottomRight, face));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, getUV(topLeft, face));
    insertVec3(m_vertexData, getPu(topLeft, face));
    insertVec3(m_vertexData, getPv(topLeft, face));


    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, getUV(bottomRight, face));
    insertVec3(m_vertexData, getPu(bottomRight, face));
    insertVec3(m_vertexData, getPv(bottomRight, face));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, getUV(topRight, face));
    insertVec3(m_vertexData, getPu(topRight, face));
    insertVec3(m_vertexData, getPv(topRight, face));

}

glm::vec2 Cube::getUV(glm::vec3 vert, int face){

    glm::vec2 uv(1.0f);

    //each face of cube correlates to an int
    //front: 0, back 1, top 2, bottom 3, left 4, right 5

    if(face == 5){
        uv = glm::vec2(- vert.z + 0.5f, vert.y + 0.5f);

    } else if(face == 4) {
        uv = glm::vec2(vert.z + 0.5f, vert.y + 0.5f);

    } else if(face == 2) {
        uv = glm::vec2(vert.x + 0.5f, - vert.z + 0.5f);

    } else if(face == 3) {
        uv = glm::vec2(vert.x + 0.5f, vert.z + 0.5f);

    } else if(face == 0) {
        uv = glm::vec2(vert.x + 0.5f, vert.y + 0.5f);

    } else if(face == 1) {
        uv = glm::vec2(- vert.x + 0.5f, vert.y + 0.5f);

    }

    return uv;
}

glm::vec3 Cube::getPu(glm::vec3 vert, int face){

    glm::vec3 Pu(1.0f);

    //each face of cube correlates to an int
    //front: 0, back 1, top 2, bottom 3, left 4, right 5



    if(face == 5){
        Pu = glm::vec3(0, 0, -1.0f);

    } else if(face == 4) {
        Pu = glm::vec3(0, 0, 1.0f);

    } else if(face == 2) {
        Pu = glm::vec3(1.0f, 0, 0);

    } else if(face == 3) {
        Pu = glm::vec3(1.0f, 0, 0);

    } else if(face == 0) {
        Pu = glm::vec3(1.0f, 0, 0);

    } else if(face == 1) {
        Pu = glm::vec3(-1.0f, 0, 0);
    }

    return Pu;

}

glm::vec3 Cube::getPv(glm::vec3 vert, int face){

    glm::vec3 Pv(1.0f);

    //front: 0, back 1, top 2, bottom 3, left 4, right 5


    if(face == 5){
        Pv = glm::vec3(0, 1.0f, 0);

    } else if(face == 4) {
        Pv = glm::vec3(0, 1.0f, 0);

    } else if(face == 2) {
        Pv = glm::vec3(0, 0, -1.0f);

    } else if(face == 3) {
        Pv = glm::vec3(0, 0, 1.0f);

    } else if(face == 0) {
        Pv = glm::vec3(0, 1.0f, 0);

    } else if(face == 1) {
        Pv = glm::vec3(0, 1.0f, 0);
    }


    return Pv;

}


void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight,
                    int face) {


    glm::vec3 right = glm::vec3(topRight - topLeft) / float(m_param1);
    glm::vec3 down = glm::vec3(bottomLeft - topLeft) / float(m_param1);

    float u0; float u1; float v0; float v1;

    for (int i = 0; i < m_param1; i++){
        for(int j = 0; j< m_param1; j++){

            glm::vec3 topLeftTiled = topLeft + float(i) * right + float(j) * down;
            glm::vec3 topRightTiled = topLeft + float(i + 1) * right + float(j) * down;
            glm::vec3 bottomLeftTiled = topLeft + float(i) * right + float(j + 1) * down;
            glm::vec3 bottomRightTiled = topLeft + float(i + 1) * right + float(j + 1) * down;

            makeTile(topLeftTiled, topRightTiled, bottomLeftTiled, bottomRightTiled, face);

        }
    }


}

void Cube::setVertexData() {

    //each face of cube correlates to an int
    //front: 0, back 1, top 2, bottom 3, left 4, right 5


    // front
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f), 0);

    // back
    makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f), 1);

    // top
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3( 0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3( 0.5f, 0.5f, 0.5f), 2);

    // bottom
    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3( 0.5f, -0.5f, -0.5f), 3);

    //left
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f), 4);

    //right
    makeFace(glm::vec3(0.5f,  0.5f,  0.5f),
             glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f,  0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f), 5);


}

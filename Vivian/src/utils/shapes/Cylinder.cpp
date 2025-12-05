#include "Cylinder.h"
#include "numbers"
#include <iostream>

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    if(m_param2 < 3){
        m_param2 = 3;
    }
    setVertexData();
}

glm::mat3 Cylinder::inertiaTensor(float m, glm::vec3 scale){
    float r = scale.x * 0.5f;
    float h = scale.y;
    float I_xz = (1.0f/12.0f) * m * (3*r*r + h*h);
    float I_y  = 0.5f * m * (r*r);
    return glm::mat3(
        I_xz, 0,    0,
        0,    I_y,  0,
        0,    0,    I_xz
        );
}

void Cylinder::makeCapTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight) {

    glm::vec3 normal;

    if(topLeft.y < 0){ normal = glm::vec3 (0, -1, 0);}
    else{ normal = glm::vec3 (0, 1, 0);}

    //top cap
    if(topLeft.y == 0.5f){
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(topLeft.x + 0.5f, -topLeft.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, -1.0f));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(bottomLeft.x + 0.5f, -bottomLeft.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, -1.0f));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(topRight.x + 0.5f, -topRight.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, -1.0f));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(topRight.x + 0.5f, -topRight.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, -1.0f));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(bottomLeft.x + 0.5f, -bottomLeft.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, -1.0f));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(bottomRight.x + 0.5f, -bottomRight.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, -1.0f));

    } else { //bottom cap

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(topLeft.x + 0.5f, topLeft.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(bottomLeft.x + 0.5f, bottomLeft.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(topRight.x + 0.5f, topRight.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(topRight.x + 0.5f, topRight.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(bottomLeft.x + 0.5f, bottomLeft.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, glm::vec2(bottomRight.x + 0.5f, bottomRight.z + 0.5f));
        insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
        insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

    }



}

void Cylinder::makeSideTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight) {

    glm::vec3 normal = glm::normalize(glm::vec3((topLeft.x + topRight.x) / 2, 0, (topLeft.z + topRight.z) / 2));

    float left_theta = atan2(bottomLeft.z, bottomLeft.x);
    float right_theta = atan2(bottomRight.z, bottomRight.x);
    float left_u; float right_u;

    if(left_theta < 0){
        left_u = - left_theta / (2.0f * std::numbers::pi);
    } else {
        left_u = 1 - left_theta / (2.0f * std::numbers::pi);
    }

    if(right_theta < 0){
        right_u = - right_theta / (2.0f * std::numbers::pi);
    }else {
        right_u = 1 - right_theta / (2.0f * std::numbers::pi);
    }

    //seam fix

    //left is near 1.0, right is near 0.0 (wrap-around)
    if (left_u > 0.9f && right_u < (0.1f)) {
        right_u += 1.0f;
    }

    //right near 1.0, left near 0.0
    if (right_u > 0.9f && left_u < (0.1f)) {
        left_u += 1.0f;
    }

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z)));
    insertVec2(m_vertexData, glm::vec2(left_u, topLeft.y + 0.5f));
    insertVec3(m_vertexData, getPu(topLeft));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z)));
    insertVec2(m_vertexData, glm::vec2(left_u, bottomLeft.y + 0.5f));
    insertVec3(m_vertexData, getPu(bottomLeft));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(topRight.x, 0, topRight.z)));
    insertVec2(m_vertexData, glm::vec2(right_u, topRight.y + 0.5f));
    insertVec3(m_vertexData, getPu(topRight));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));


    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(topRight.x, 0, topRight.z)));
    insertVec2(m_vertexData, glm::vec2(right_u, topRight.y + 0.5f));
    insertVec3(m_vertexData, getPu(topRight));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z)));
    insertVec2(m_vertexData, glm::vec2(left_u, bottomLeft.y + 0.5f));
    insertVec3(m_vertexData, getPu(bottomLeft));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z)));
    insertVec2(m_vertexData, glm::vec2(right_u, bottomRight.y + 0.5f));
    insertVec3(m_vertexData, getPu(bottomRight));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));


}

glm::vec3 Cylinder::getPu(glm::vec3 vertex){

    return glm::vec3(vertex.z / ( 2 * std::numbers::pi * (vertex.x * vertex.x + vertex.z * vertex.z)),
                     0,
                     -vertex.x / ( 2 * std::numbers::pi * (vertex.x * vertex.x + vertex.z * vertex.z)));

}

void Cylinder::makeCapSlice(float currentTheta, float nextTheta){

    float increment = 0.5f / float (m_param1);
    float radius = 0.5f;

    for(int i = 0; i < m_param1; i++){

        glm::vec3 topLeft = glm::vec3(radius * cos(nextTheta), -0.5f, radius * sin(nextTheta));
        glm::vec3 topRight = glm::vec3(radius * cos(currentTheta), -0.5f, radius * sin(currentTheta));

        glm::vec3 bottomLeft = glm::vec3((radius - increment) * cos(nextTheta), -0.5f, (radius - increment) * sin(nextTheta));
        glm::vec3 bottomRight = glm::vec3((radius - increment) * cos(currentTheta), -0.5f, (radius - increment) * sin(currentTheta));

        makeCapTile(topLeft, topRight, bottomLeft, bottomRight);

        radius -= increment;

    }

    radius = 0.5f;

    for(int i = 0; i < m_param1; i++){

        glm::vec3 topLeft = glm::vec3(radius * cos(currentTheta), 0.5f, radius * sin(currentTheta));
        glm::vec3 topRight = glm::vec3(radius * cos(nextTheta), 0.5f, radius * sin(nextTheta));

        glm::vec3 bottomLeft = glm::vec3((radius - increment) * cos(currentTheta), 0.5f, (radius - increment) * sin(currentTheta));
        glm::vec3 bottomRight = glm::vec3((radius - increment) * cos(nextTheta), 0.5f, (radius - increment) * sin(nextTheta));

        makeCapTile(topLeft, topRight, bottomLeft, bottomRight);

        radius -= increment;

    }



}

void Cylinder::makeSideSlice(float currentTheta, float nextTheta){

    float heightIncrement = 1.0f / m_param1;

    float h = 0.5f;
    float r = 0.5f;

    for(int i = 0; i < m_param1; i++){

        glm::vec3 topLeft = glm::vec3(r * cos(nextTheta), h, r * sin(nextTheta));
        glm::vec3 topRight = glm::vec3(r * cos(currentTheta), h, r * sin(currentTheta));

        glm::vec3 bottomLeft = glm::vec3(r * cos(nextTheta), h - heightIncrement, r * sin(nextTheta));
        glm::vec3 bottomRight = glm::vec3(r * cos(currentTheta), h - heightIncrement, r * sin(currentTheta));

        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);

        h -= heightIncrement;

    }
}


void Cylinder::makeWedge(float currentTheta, float nextTheta) {

    makeSideSlice(currentTheta, nextTheta);
    makeCapSlice(currentTheta, nextTheta);

}


void Cylinder::setVertexData() {

    float thetaIncrement = 2 * std::numbers::pi / m_param2;
    float r = 0;

    for(int i = 0; i < m_param2; i++){

        makeWedge(r, r + thetaIncrement);

        r += thetaIncrement;
    }
}


void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cylinder::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}


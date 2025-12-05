#include "Cone.h"
#include <numbers>
#include <cmath>

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    if(m_param2 < 3){
        m_param2 = 3;
    }
    setVertexData();
}

glm::mat3 Cone::inertiaTensor(float m, glm::vec3 scale){
    float r = scale.x * 0.5f;
    float h = scale.y;
    float I_xz = (3.0f/20.0f) * m * (r*r + 0.25f*h*h);
    float I_y  = (3.0f/10.0f)  * m * r*r;
    return glm::mat3(
        I_xz, 0,    0,
        0,    I_y,  0,
        0,    0,    I_xz
        );
}

void Cone::makeCapTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight) {

    glm::vec3 normal = glm::vec3(0, -1, 0);

    //add uv values to shapes
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
    insertVec2(m_vertexData, glm::vec2(topRight.x + 0.5, topRight.z + 0.5));
    insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
    insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, glm::vec2(bottomLeft.x + 0.5, bottomLeft.z + 0.5));
    insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
    insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, glm::vec2(bottomRight.x + 0.5, bottomRight.z + 0.5));
    insertVec3(m_vertexData, glm::vec3(1.0f, 0, 0));
    insertVec3(m_vertexData, glm::vec3(0, 0, 1.0f));
}

void Cone::makeSlopeTile(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight) {

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


    //fix seams

    //left is near 1.0, right is near 0.0 (wrap-around)
    if (left_u > 0.9f && right_u < (0.1f)) {
        right_u += 1.0f;
    }

    //right near 1.0, left near 0.0
    if (right_u > 0.9f && left_u < (0.1f)) {
        left_u += 1.0f;
    }

    //for each vertex, add vec3 position, vec3 normals, vec2 uv values, vec3 dp/du, and vec3 dp/dv

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2 * bottomLeft.x, 0.5f * (0.5f - bottomLeft.y), 2 * bottomLeft.z)));
    insertVec2(m_vertexData, glm::vec2(left_u, topLeft.y + 0.5f));
    insertVec3(m_vertexData, getPu(topLeft));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2 * bottomLeft.x, 0.5f * (0.5f - bottomLeft.y), 2 * bottomLeft.z)));
    insertVec2(m_vertexData, glm::vec2(left_u, bottomLeft.y + 0.5f));
    insertVec3(m_vertexData, getPu(bottomLeft));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2 * bottomRight.x, 0.5f * (0.5f - bottomRight.y), 2 * bottomRight.z)));
    insertVec2(m_vertexData, glm::vec2(right_u, topRight.y + 0.5f));
    insertVec3(m_vertexData, getPu(topRight));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2 * bottomRight.x, 0.5f * (0.5f - bottomRight.y), 2 * bottomRight.z)));
    insertVec2(m_vertexData, glm::vec2(right_u, topRight.y + 0.5f));
    insertVec3(m_vertexData, getPu(topRight));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2 * bottomLeft.x, 0.5f * (0.5f - bottomLeft.y), 2 * bottomLeft.z)));
    insertVec2(m_vertexData, glm::vec2(left_u, bottomLeft.y + 0.5f));
    insertVec3(m_vertexData, getPu(bottomLeft));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2 * bottomRight.x, 0.5f * (0.5f - bottomRight.y), 2 * bottomRight.z)));
    insertVec2(m_vertexData, glm::vec2(right_u, bottomRight.y + 0.5f));
    insertVec3(m_vertexData, getPu(bottomRight));
    insertVec3(m_vertexData, glm::vec3(0, 1.0f, 0));


}

glm::vec3 Cone::getPu(glm::vec3 vertex){

    if(vertex.x * vertex.x + vertex.z * vertex.z == 0){
        return glm::vec3( 0, 0, 0);
    }

    return glm::vec3(vertex.z / ( 2 * std::numbers::pi * (vertex.x * vertex.x + vertex.z * vertex.z)),
                     0,
                     -vertex.x / ( 2 * std::numbers::pi * (vertex.x * vertex.x + vertex.z * vertex.z)));

}

void Cone::makeCapSlice(float currentTheta, float nextTheta){

    glm::vec3 normal = glm::vec3(0, 0, -1);
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
}

void Cone::makeSlopeSlice(float currentTheta, float nextTheta){

    float heightIncrement = 1.0f / m_param1;
    float radiusIncrement = 0.5f / m_param1;

    float h = 0.5f;
    float r = 0;


    for(int i = 0; i < m_param1; i++){
        glm::vec3 topLeft = glm::vec3(r * cos(nextTheta), h, r * sin(nextTheta));
        glm::vec3 topRight = glm::vec3(r * cos(currentTheta), h, r * sin(currentTheta));

        glm::vec3 bottomLeft = glm::vec3((r + radiusIncrement) * cos(nextTheta), h - heightIncrement, (r + radiusIncrement) * sin(nextTheta));
        glm::vec3 bottomRight =  glm::vec3((r + radiusIncrement) * cos(currentTheta), h - heightIncrement, (r + radiusIncrement) * sin(currentTheta));



        makeSlopeTile(topLeft, topRight, bottomLeft, bottomRight);

        h -= heightIncrement;
        r += radiusIncrement;

    }



}

void Cone::makeWedge(float currentTheta, float nextTheta) {

    makeSlopeSlice(currentTheta, nextTheta);
    makeCapSlice(currentTheta, nextTheta);

}

void Cone::setVertexData() {

    float thetaIncrement = 2 * std::numbers::pi / m_param2;
    float r = 0;

    for(int i = 0; i < m_param2; i++){

        makeWedge(r, r + thetaIncrement);

        r += thetaIncrement;
    }

}

void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cone::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

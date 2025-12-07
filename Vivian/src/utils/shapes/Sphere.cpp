#include "Sphere.h"
#include <numbers>

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    if(m_param1 < 2){
        m_param1 = 2;
    }
    m_param2 = param2;
    if(m_param2 < 3){
        m_param2 = 3;
    }
    setVertexData();
}

glm::mat3 Sphere::inertiaTensor(float m, glm::vec3 scale){
    float r = scale.x * 0.5f;
    float coeff = 0.4f * m * r * r;   // 2/5 m r^2
    return glm::mat3(coeff);
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

    glm::vec2 topLeft_uv = getUV(topLeft);
    glm::vec2 topRight_uv = getUV(topRight);
    glm::vec2 bottomLeft_uv = getUV(bottomLeft);
    glm::vec2 bottomRight_uv = getUV(bottomRight);

    //seam fix
    //left is near 1.0, right is near 0.0 (wrap-around)
    if ((bottomLeft_uv.x > 0.7f || topLeft_uv.x > 0.7f) && (bottomRight_uv.x < (0.3f) || topRight_uv.x < 0.3f)) {
        bottomRight_uv.x += 1.0f;
        topRight_uv.x += 1.0f;
    }

    //right near 1.0, left near 0.0
    if ((bottomRight_uv.x > 0.7f || topRight_uv.x > 0.7f) && (bottomLeft_uv.x < (0.3f) || topLeft_uv.x < 0.3f)) {
        bottomLeft_uv.x += 1.0f;
        topLeft_uv.x += 1.0f;
    }


    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec2(m_vertexData, topLeft_uv);
    insertVec3(m_vertexData, getPu(topLeft));
    insertVec3(m_vertexData, getPv(topLeft));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec2(m_vertexData, bottomLeft_uv);
    insertVec3(m_vertexData, getPu(bottomLeft));
    insertVec3(m_vertexData, getPv(bottomLeft));


    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec2(m_vertexData, topRight_uv);
    insertVec3(m_vertexData, getPu(topRight));
    insertVec3(m_vertexData, getPv(topRight));


    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec2(m_vertexData, topRight_uv);
    insertVec3(m_vertexData, getPu(topRight));
    insertVec3(m_vertexData, getPv(topRight));


    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec2(m_vertexData, bottomLeft_uv);
    insertVec3(m_vertexData, getPu(bottomLeft));
    insertVec3(m_vertexData, getPv(bottomLeft));


    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec2(m_vertexData, bottomRight_uv);
    insertVec3(m_vertexData, getPu(bottomRight));
    insertVec3(m_vertexData, getPv(bottomRight));


}

glm::vec2 Sphere::getUV(glm::vec3 vertex){
    float theta = atan2(vertex.z, vertex.x);
    float u;
    if(theta < 0){
        u =  - theta  / (2.0f * std::numbers::pi);
    } else{
        u = 1 - (theta/ (2.0f * std::numbers::pi));
    }

    float phi = asin(vertex.y / 0.5f);
    float v = (phi/std::numbers::pi) + 0.5f;

    return glm::vec2(u, v);
}

glm::vec3 Sphere::getPu(glm::vec3 vertex){

    if(vertex.x * vertex.x + vertex.z * vertex.z == 0){
        return glm::vec3( 0, 0, 0);
    }

    if(vertex.y == -0.5f){
        return glm::vec3(0, 0, -1);
    }

    return glm::vec3(vertex.z / ( 2 * std::numbers::pi * (vertex.x * vertex.x + vertex.z * vertex.z)),
                     0,
                     -vertex.x / ( 2 * std::numbers::pi * (vertex.x * vertex.x + vertex.z * vertex.z)));

}

glm::vec3 Sphere::getPv(glm::vec3 vertex){

    if(vertex.y == 0.5f){
        return glm::vec3(0, 0, 1);
    }

    if(vertex.y == -0.5f){
        return glm::vec3(-1, 0, 0);
    }

    return glm::vec3(0, 1.0f / (std::numbers::pi * std::sqrt(0.25f - vertex.y * vertex.y)), 0);

}


void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    for(int i = 0; i < m_param1; i++){

        float phiTop = std::numbers::pi * i / m_param1;
        float phiBottom = std::numbers::pi * (i + 1) / m_param1;

        glm::vec3 topLeft = glm::vec3(0.5f * sin(phiTop) * cos(currentTheta),
                                      0.5f * cos(phiTop),
                                      -0.5f * sin(phiTop) * sin(currentTheta));

        glm::vec3 topRight = glm::vec3(0.5f * sin(phiTop ) * cos(nextTheta),
                                       0.5f * cos(phiTop ),
                                      -0.5f * sin(phiTop) * sin(nextTheta));

        glm::vec3 bottomLeft = glm::vec3(0.5f * sin(phiBottom) * cos(currentTheta),
                                         0.5f * cos(phiBottom),
                                         -0.5f * sin(phiBottom) * sin(currentTheta));

        glm::vec3 bottomRight = glm::vec3(0.5f * sin(phiBottom) * cos(nextTheta),
                                         0.5f * cos(phiBottom),
                                         -0.5f * sin(phiBottom) * sin(nextTheta));

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!

    float currentTheta = 0;
    float thetaStep = 2 * std::numbers::pi / m_param2;



    for(int i = 0; i < m_param2; i++){

        makeWedge(currentTheta, currentTheta + thetaStep);
        currentTheta += thetaStep;

    }
}

void Sphere::setVertexData() {

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Sphere::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}


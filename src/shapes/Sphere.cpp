#include "Sphere.h"
#include <numbers>

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    if (param1<3) m_param1 = 3;
    else m_param1 = param1;
    if (param2<3) m_param2 = 3;
    else m_param2 = param2;
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


    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));


    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));



}

void Sphere::makeWedge(float currentTheta, float nextTheta) { //pi is half of the circle
    for(int i = 0; i<m_param1; i++){
        float currentPhi = (std::numbers::pi *(float)i) / m_param1;
        float nextPhi = (std::numbers::pi *(float)(i+1.f)) / m_param1;
            glm::vec3 topLeft (0.5f * glm::sin(currentPhi)*glm::cos(currentTheta),
                          0.5f * glm::cos(currentPhi),
                         -0.5f *  glm::sin(currentPhi)*glm::sin(currentTheta));

            glm::vec3 topRight(0.5f * glm::sin(nextPhi)*glm::cos(currentTheta),
                           0.5f *  glm::cos(nextPhi),
                          -0.5f *  glm::sin(nextPhi)*glm::sin(currentTheta));

            glm::vec3 bottomLeft (0.5f * glm::sin(currentPhi)*glm::cos(nextTheta),
                              0.5f * glm::cos(currentPhi), -0.5f * glm::sin(currentPhi)*glm::sin(nextTheta));

            glm::vec3 bottomRight (0.5f * glm::sin(nextPhi)*glm::cos(nextTheta),
                            0.5f * glm::cos(nextPhi), -0.5f * glm::sin(nextPhi) * glm::sin(nextTheta));

        makeTile(bottomLeft, bottomRight, topLeft, topRight);

        }
    }

void Sphere::makeSphere() {
    float thetaStep = glm::radians(360.f / m_param2);
    for(int i = 0; i<m_param2;i++){
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    makeSphere();
}


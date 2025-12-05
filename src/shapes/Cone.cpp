#include "Cone.h"
#include <numbers>

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    if (param2<3) m_param2 = 3;
    else m_param2 = param2;
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

void Cone::makeCapTile(glm::vec3 topLeft,glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight){
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

void Cone::makeSlopeTile(glm::vec3 topLeft,glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight){
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,calcNorm(topLeft));
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData, calcNorm(bottomRight));
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,calcNorm(bottomLeft));

    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,calcNorm(topRight));

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,calcNorm(bottomRight));
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,calcNorm(topLeft));
}



glm::vec3 Cone::calcNorm(glm::vec3& pt) {
    const float epsilon = 1e-4f;
    float xNorm = (2 * pt.x);
    float yNorm = -(1.f / 4.f) * (2.f * pt.y - 1.f);
    float zNorm = (2 * pt.z);
    glm::vec3 norm = glm::normalize(glm::vec3{ xNorm, yNorm, zNorm });
    return norm;
}

void Cone::makeCapSlice(float currentTheta, float nextTheta){
    // Note: think about how param 1 comes into play here!
        float baseY = -0.5f;     // base of cone
        float baseRadius = 0.5f; // radius of base
        for (int i = 0; i < m_param1; i++) {
            float innerR = (baseRadius * i) / m_param1;
            float outerR = (baseRadius * (i + 1)) / m_param1;

            glm::vec3 topLeft(
                innerR * cos(currentTheta),
                baseY,
                innerR * sin(currentTheta)
                );

            glm::vec3 topRight(
                innerR * cos(nextTheta),
                baseY,
                innerR * sin(nextTheta)
                );

            glm::vec3 bottomLeft(
                outerR * cos(currentTheta),
                baseY,
                outerR * sin(currentTheta)
                );

            glm::vec3 bottomRight(
                outerR * cos(nextTheta),
                baseY,
                outerR * sin(nextTheta)
                );
            makeCapTile(topLeft, topRight, bottomLeft, bottomRight);
    }

}

void Cone::makeSlopeSlice(float currentTheta, float nextTheta){
        float baseY = -0.5f;
        float apexY = 0.5f;
        float baseRadius = 0.5f;

        // For each vertical subdivision
        for (int i = 0; i < m_param1; i++) {
            float y1 = baseY + (apexY - baseY) * (i / (float)m_param1);
            float y2 = baseY + (apexY - baseY) * ((i + 1) / (float)m_param1);
            float r1 = baseRadius * (1 - i / (float)m_param1);
            float r2 = baseRadius * (1 - (i + 1) / (float)m_param1);

            glm::vec3 bottomLeft(r1 * cos(currentTheta), y1, r1 * sin(currentTheta));
            glm::vec3 bottomRight(r1 * cos(nextTheta), y1, r1 * sin(nextTheta));
            glm::vec3 topLeft(r2 * cos(currentTheta), y2, r2 * sin(currentTheta));
            glm::vec3 topRight(r2 * cos(nextTheta), y2, r2 * sin(nextTheta));
            if (r2 < 1e-4f || r2 < 1e-4f){
                glm::vec3 baseNorm1 = calcNorm(bottomLeft);
                glm::vec3 baseNorm2 = calcNorm(bottomRight);
                glm::vec3 avgBaseNorm = glm::normalize(baseNorm1 + baseNorm2);
                insertVec3(m_vertexData,topLeft);
                insertVec3(m_vertexData,avgBaseNorm);
                insertVec3(m_vertexData,bottomRight);
                insertVec3(m_vertexData, calcNorm(bottomRight));
                insertVec3(m_vertexData,bottomLeft);
                insertVec3(m_vertexData,calcNorm(bottomLeft));

                insertVec3(m_vertexData,topRight);
                insertVec3(m_vertexData,avgBaseNorm);

                insertVec3(m_vertexData,bottomRight);
                insertVec3(m_vertexData,calcNorm(bottomRight));
                insertVec3(m_vertexData,topLeft);
                insertVec3(m_vertexData,avgBaseNorm);
            }
            else{

                makeSlopeTile(topLeft, topRight, bottomLeft, bottomRight);
            }
    }

}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    makeCapSlice(currentTheta, nextTheta);
    makeSlopeSlice(currentTheta, nextTheta);


}

void Cone::setVertexData() {
    float thetaStep = glm::radians(360.f / m_param2);
    for(int i = 0; i < m_param2;i++){
        float currentTheta = i * thetaStep;
        float nextTheta = (i+1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

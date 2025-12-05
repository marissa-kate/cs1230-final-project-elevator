#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2){
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    if (param2 < 3) m_param2 = 3;
    else m_param2 = param2;
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
void Cylinder::setVertexData() {
    makeCylinder();
}

void Cylinder::makeCapTile(glm::vec3 topLeft,glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight){
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

void Cylinder::makeCapSlice(float currentTheta, float nextTheta, bool topCap){
    float y = -0.5f;
    // Task 8: create a slice of the cap face using your
    //         make tile function(s)
    // Note: think about how param 1 comes into play here!
    if(topCap) y = 0.5f;
    // base of cone
    float baseRadius = 0.5f; // radius of base
    for (int i = 0; i < m_param1; i++) {
        float innerR = (baseRadius * i) / m_param1;
        float outerR = (baseRadius * (i + 1)) / m_param1;

        glm::vec3 topLeft(
            innerR * cos(currentTheta),
            y,
            innerR * sin(currentTheta)
            );

        glm::vec3 topRight(
            innerR * cos(nextTheta),
            y,
            innerR * sin(nextTheta)
            );

        glm::vec3 bottomLeft(
            outerR * cos(currentTheta),
            y,
            outerR * sin(currentTheta)
            );

        glm::vec3 bottomRight(
            outerR * cos(nextTheta),
            y,
            outerR * sin(nextTheta)
            );

        // Create the tile for this section of the cap
        if(topCap)makeCapTile(topRight, topLeft, bottomRight, bottomLeft);
        else makeCapTile(topLeft, topRight, bottomLeft, bottomRight);
    }

}

void Cylinder::makeSideTile(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normalLeft  = glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z));
    glm::vec3 normalRight = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    glm::vec3 normalBottomLeft  = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    glm::vec3 normalBottomRight = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));


    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,normalLeft);
    insertVec3(m_vertexData,bottomLeft);
    insertVec3(m_vertexData,normalBottomLeft);
    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData, normalBottomRight);

    insertVec3(m_vertexData,bottomRight);
    insertVec3(m_vertexData,normalBottomRight);
    insertVec3(m_vertexData,topRight);
    insertVec3(m_vertexData,normalRight);
    insertVec3(m_vertexData,topLeft);
    insertVec3(m_vertexData,normalLeft);


}


void Cylinder::makeSideSlice(float currentTheta, float nextTheta) {
    float topY = 0.5f;
    float bottomY = -0.5f;
    float radius = 0.5f;

    for (int i = 0; i < m_param1; i++) {
        float v1 = (float)i / m_param1;
        float v2 = (float)(i + 1) / m_param1;

        float yTop = glm::mix(topY, bottomY, v1);
        float yBottom = glm::mix(topY, bottomY, v2);

        glm::vec3 topLeft(radius * cos(currentTheta), yTop, radius * sin(currentTheta));
        glm::vec3 topRight(radius * cos(nextTheta), yTop, radius * sin(nextTheta));
        glm::vec3 bottomLeft(radius * cos(currentTheta), yBottom, radius * sin(currentTheta));
        glm::vec3 bottomRight(radius * cos(nextTheta), yBottom, radius * sin(nextTheta));

        makeSideTile( bottomLeft, bottomRight, topLeft, topRight);
    }
}


void Cylinder::makeCylinder(){
    float thetaStep = glm::radians(360.0f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeSideSlice(currentTheta, nextTheta);
        makeCapSlice(currentTheta, nextTheta, true);
        makeCapSlice(currentTheta, nextTheta, false);
    }
}

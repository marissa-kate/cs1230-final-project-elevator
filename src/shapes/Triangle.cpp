#include "Triangle.h"

void Triangle::updateParams(int param1, int param2){
    m_vertexData = std::vector<float>();
    setVertexData();
}

void Triangle::setVertexData() {
    glm::vec3 p1 (-0.5, 0.5, 0.0);
    glm::vec3 p2 (-0.5, -0.5, 0.0);
    glm::vec3 p3 (0.5, -0.5, 0.0);
    glm::vec3 normal(0,0,1);

    insertVec3(m_vertexData, p1);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, normal);
}

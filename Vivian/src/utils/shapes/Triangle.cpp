#include "Triangle.h"

void Triangle::updateParams() {
    m_vertexData = std::vector<float>();
    setVertexData();
}

void Triangle::setVertexData() {


    //DID NOT UPDATE TRIANGLES WITH UVS OR DP, DV values

    //top
    insertVec3(m_vertexData, glm::vec3(-0.5, 0.5, 0));
    insertVec3(m_vertexData, glm::vec3(0, 0, 1));


    //bottom left
    insertVec3(m_vertexData, glm::vec3(-0.5, -0.5, 0));
    insertVec3(m_vertexData, glm::vec3( 0, 0, 1));


    //bottom right
    insertVec3(m_vertexData, glm::vec3(0.5, -0.5, 0));
    insertVec3(m_vertexData, glm::vec3( 0, 0, 1));

}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Triangle::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

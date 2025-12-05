#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cylinder::setVertexData() {
    for (int i = 0; i < m_param2; ++i) {
        float curr_theta = (float)i * (2.0f * M_PI / float(m_param2));
        float next_theta = ((float)i + 1.0f) * (2.0f * M_PI / float(m_param2));
        makeWedge(curr_theta, next_theta);
    }
}
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cylinder::makeCapSlice(glm::vec3 one, glm::vec3 two, glm::vec3 three, glm::vec3 four, glm::vec3 normal) {
    insertVec3(m_vertexData, two);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, one);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, three);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, two);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, three);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, four);
    insertVec3(m_vertexData, normal);
}

glm::vec3 Cylinder::calcNorm(glm::vec3& pt) {
    float xNorm = (2 * pt.x);
    float yNorm = 0.0f;
    float zNorm = (2 * pt.z);

    return glm::normalize(glm::vec3{ xNorm, yNorm, zNorm });
}

void Cylinder::makeSlopeTile(glm::vec3 topLeft,
                             glm::vec3 topRight,
                             glm::vec3 bottomLeft,
                             glm::vec3 bottomRight,
                             glm::vec3 normal) {

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, calcNorm(topLeft));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, calcNorm(topRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, calcNorm(bottomRight));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, calcNorm(topLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, calcNorm(bottomRight));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, calcNorm(bottomLeft));
}

void Cylinder::makeSlopeSlice(float currentTheta, float nextTheta){
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 topLeft = glm::vec3((m_radius * cos(currentTheta)), (1.0f - (i/float(m_param1))) - 0.5f , (m_radius * sin(currentTheta)));
        glm::vec3 topRight = glm::vec3(((m_radius) * cos(nextTheta)), (1.0f - (i/float(m_param1))) - 0.5f , (m_radius * sin(nextTheta)));
        glm::vec3 bottomLeft = glm::vec3(((m_radius) * cos(currentTheta)), (1.0f - ((i+1)/float(m_param1))) - 0.5f , ((m_radius * sin(currentTheta))));
        glm::vec3 bottomRight = glm::vec3(((m_radius) * cos(nextTheta)), (1.0f - ((i+1)/float(m_param1))) - 0.5f , (( m_radius * sin(nextTheta))));
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, i * currentTheta);
        makeSlopeTile(topLeft, topRight, bottomLeft, bottomRight, normal);
    }
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    glm::vec3 top_cap_normal = glm::vec3(0, 1, 0);
    glm::vec3 bottom_cap_normal = glm::vec3(0, -1, 0);
    for (int i = 0; i < m_param1; ++i) {

        float y_bottom_coord = -0.5;
        float y_top_coord = 0.5;

        float x_topLeft_bottom = (i + 1) / float(m_param1) * m_radius * cos(currentTheta);
        float z_topLeft_bottom = (i + 1) / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 one = glm::vec3(x_topLeft_bottom, y_top_coord, z_topLeft_bottom);

        float x_topRight_bottom = (i + 1) / float(m_param1) * m_radius * cos(nextTheta);
        float z_topRight_bottom  = (i + 1) / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 two  = glm::vec3(x_topRight_bottom, y_top_coord, z_topRight_bottom);

        float x_bottomLeft_bottom  = i / float(m_param1) * m_radius * cos(currentTheta);
        float z_bottomLeft_bottom  = i / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 three  = glm::vec3(x_bottomLeft_bottom, y_top_coord, z_bottomLeft_bottom);

        float x_bottomRight_bottom  = i / float(m_param1) * m_radius * cos(nextTheta);
        float z_bottomRight_bottom  = i / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 four  = glm::vec3(x_bottomRight_bottom, y_top_coord, z_bottomRight_bottom);

        makeCapSlice(one, two, three, four, top_cap_normal);

        glm::vec3 new_one = glm::vec3(one.x, y_bottom_coord, one.z);
        glm::vec3 new_two = glm::vec3(two.x, y_bottom_coord, two.z);
        glm::vec3 new_three = glm::vec3(three.x, y_bottom_coord, three.z);
        glm::vec3 new_four = glm::vec3(four.x, y_bottom_coord, four.z);

        makeCapSlice(new_two, new_one, new_four, new_three, bottom_cap_normal);

        makeSlopeSlice(currentTheta, nextTheta);

    }
}



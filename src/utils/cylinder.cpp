#include "cylinder.h"
#include "shapes.h"
#include <iostream>
#include <glm/glm.hpp>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

cylinder::cylinder() {}

void cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void cylinder::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

void cylinder::setCylinderVertexData() {
    for (int i = 0; i < m_param2; ++i) {
        float curr_theta = (float)i * (2.0f * M_PI / float(m_param2));
        float next_theta = ((float)i + 1.0f) * (2.0f * M_PI / float(m_param2));
        makeCylinderWedge(curr_theta, next_theta);
    }
}

void cylinder::makeCylinderCapSlice(glm::vec3 one, glm::vec2 one_uv,
                                    glm::vec3 two, glm::vec2 two_uv,
                                    glm::vec3 three, glm::vec2 three_uv,
                                    glm::vec3 four, glm::vec2 four_uv,
                                    glm::vec3 normal) {
    insertVec3(m_vertexData, two);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, two_uv);
    insertVec3(m_vertexData, one);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, one_uv);
    insertVec3(m_vertexData, three);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, three_uv);

    insertVec3(m_vertexData, two);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, two_uv);
    insertVec3(m_vertexData, three);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, three_uv);
    insertVec3(m_vertexData, four);
    insertVec3(m_vertexData, normal);
    insertVec2(m_vertexData, four_uv);
}

glm::vec3 cylinder::calcCylinderNorm(glm::vec3& pt) {
    float xNorm = (2 * pt.x);
    float yNorm = 0.0f;
    float zNorm = (2 * pt.z);

    return glm::normalize(glm::vec3{ xNorm, yNorm, zNorm });
}

void cylinder::makeCylinderSlopeTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                                   glm::vec3 topRight, glm::vec2 topRight_uv,
                                   glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                                   glm::vec3 bottomRight, glm::vec2 bottomRight_uv,
                                   glm::vec3 normal) {

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, calcCylinderNorm(topLeft));
    insertVec2(m_vertexData, topLeft_uv);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, calcCylinderNorm(topRight));
    insertVec2(m_vertexData, topRight_uv);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, calcCylinderNorm(bottomRight));
    insertVec2(m_vertexData, bottomRight_uv);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, calcCylinderNorm(topLeft));
    insertVec2(m_vertexData, topLeft_uv);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, calcCylinderNorm(bottomRight));
    insertVec2(m_vertexData, bottomRight_uv);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, calcCylinderNorm(bottomLeft));
    insertVec2(m_vertexData, bottomLeft_uv);
}

void cylinder::makeCylinderSlopeSlice(float currentTheta, float nextTheta){
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 topLeft = glm::vec3((m_radius * cos(currentTheta)), (1.0f - (i/float(m_param1))) - 0.5f , (m_radius * sin(currentTheta)));
        glm::vec2 topLeft_uv = glm::vec2((m_radius * cos(currentTheta)), (m_radius * sin(currentTheta)));
        glm::vec3 topRight = glm::vec3(((m_radius) * cos(nextTheta)), (1.0f - (i/float(m_param1))) - 0.5f , (m_radius * sin(nextTheta)));
        glm::vec2 topRight_uv = glm::vec2((m_radius * cos(nextTheta)), (m_radius * sin(nextTheta)));
        glm::vec3 bottomLeft = glm::vec3(((m_radius) * cos(currentTheta)), (1.0f - ((i+1)/float(m_param1))) - 0.5f , ((m_radius * sin(currentTheta))));
        glm::vec2 bottomLeft_uv = glm::vec2((m_radius * cos(currentTheta)), (m_radius * sin(currentTheta)));
        glm::vec3 bottomRight = glm::vec3(((m_radius) * cos(nextTheta)), (1.0f - ((i+1)/float(m_param1))) - 0.5f , (( m_radius * sin(nextTheta))));
        glm::vec2 bottomRight_uv = glm::vec2((m_radius * cos(nextTheta)), (m_radius * sin(nextTheta)));
        glm::vec3 normal = glm::vec3(0.0f, 0.0f, i * currentTheta);
        makeCylinderSlopeTile(topLeft, topLeft_uv,
                              topRight, topRight_uv,
                              bottomLeft, bottomLeft_uv,
                              bottomRight, bottomRight_uv,
                              normal);
    }
}

void cylinder::makeCylinderWedge(float currentTheta, float nextTheta) {
    glm::vec3 top_cap_normal = normalize(glm::vec3(0, 1, 0));
    glm::vec3 bottom_cap_normal = normalize(glm::vec3(0, -1, 0));
    for (int i = 0; i < m_param1; ++i) {

        float y_bottom_coord = -0.5;
        float y_top_coord = 0.5;

        float x_topLeft_bottom = (i + 1) / float(m_param1) * m_radius * cos(currentTheta);
        float z_topLeft_bottom = (i + 1) / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 one = glm::vec3(x_topLeft_bottom, y_top_coord, z_topLeft_bottom);
        glm::vec2 one_uv = glm::vec2(x_topLeft_bottom, z_topLeft_bottom);

        float x_topRight_bottom = (i + 1) / float(m_param1) * m_radius * cos(nextTheta);
        float z_topRight_bottom  = (i + 1) / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 two  = glm::vec3(x_topRight_bottom, y_top_coord, z_topRight_bottom);
        glm::vec2 two_uv = glm::vec2(x_topRight_bottom, z_topRight_bottom);

        float x_bottomLeft_bottom  = i / float(m_param1) * m_radius * cos(currentTheta);
        float z_bottomLeft_bottom  = i / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 three  = glm::vec3(x_bottomLeft_bottom, y_top_coord, z_bottomLeft_bottom);
        glm::vec2 three_uv = glm::vec2(x_bottomLeft_bottom, z_bottomLeft_bottom);

        float x_bottomRight_bottom  = i / float(m_param1) * m_radius * cos(nextTheta);
        float z_bottomRight_bottom  = i / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 four  = glm::vec3(x_bottomRight_bottom, y_top_coord, z_bottomRight_bottom);
        glm::vec2 four_uv = glm::vec2(x_bottomRight_bottom, z_bottomRight_bottom);

        makeCylinderCapSlice(one, one_uv, two, two_uv, three, three_uv, four, four_uv, top_cap_normal);

        glm::vec3 new_one = glm::vec3(one.x, y_bottom_coord, one.z);
        glm::vec2 new_one_uv = glm::vec2(one.x, one.z);
        glm::vec3 new_two = glm::vec3(two.x, y_bottom_coord, two.z);
        glm::vec2 new_two_uv = glm::vec2(two.x, two.z);
        glm::vec3 new_three = glm::vec3(three.x, y_bottom_coord, three.z);
        glm::vec2 new_three_uv = glm::vec2(three.x, three.z);
        glm::vec3 new_four = glm::vec3(four.x, y_bottom_coord, four.z);
        glm::vec2 new_four_uv = glm::vec2(four.x, four.z);

        makeCylinderCapSlice(new_two, new_two_uv,
                             new_one, new_one_uv,
                             new_four, new_four_uv,
                             new_three, new_three_uv,
                             bottom_cap_normal);

        makeCylinderSlopeSlice(currentTheta, nextTheta);

    }
}

std::vector<float> cylinder::generateCylinderData(int phiTesselations, int thetaTesselations) {
    m_vertexData = std::vector<float>();
    m_radius = 0.5;
    m_param1 = phiTesselations;
    m_param2 = thetaTesselations;
    setCylinderVertexData();
    return m_vertexData;
}

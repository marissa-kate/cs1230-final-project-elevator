#include "cone.h"

#include <iostream>
#include <glm/glm.hpp>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"


cone::cone() {}

void cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void cone::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

void cone::setConeVertexData() {
    for (int i = 0; i < m_param2; ++i) {
        float curr_theta = (float)i * (2.0f * M_PI / float(m_param2));
        float next_theta = ((float)i + 1.0f) * (2.0f * M_PI / float(m_param2));
        makeConeWedge(curr_theta, next_theta);
    }
}

void cone::makeConeCapSlice(float currentTheta, float nextTheta){
    glm::vec3 normal = glm::vec3(0.0f, -1.0f, 0.0f);
    for (int i = 0; i < m_param1; ++i) {

        float x_topLeft = (i + 1) / float(m_param1) * m_radius * cos(currentTheta);
        float z_topLeft = (i + 1) / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 topLeft = glm::vec3(x_topLeft, -0.5f, z_topLeft);
        glm::vec2 topLeft_uv = glm::vec2(x_topLeft, z_topLeft);

        float x_topRight = (i + 1) / float(m_param1) * m_radius * cos(nextTheta);
        float z_topRight = (i + 1) / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 topRight = glm::vec3(x_topRight, -0.5f, z_topRight);
        glm::vec2 topRight_uv = glm::vec2(x_topRight, z_topRight);

        float x_bottomLeft = i / float(m_param1) * m_radius * cos(currentTheta);
        float z_bottomLeft = i / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 bottomLeft = glm::vec3(x_bottomLeft, -0.5f, z_bottomLeft);
        glm::vec2 bottomLeft_uv = glm::vec2(x_bottomLeft, z_bottomLeft);

        float x_bottomRight = i / float(m_param1) * m_radius * cos(nextTheta);
        float z_bottomRight = i / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 bottomRight = glm::vec3(x_bottomRight, -0.5f, z_bottomRight);
        glm::vec2 bottomRight_uv = glm::vec2(x_bottomRight, z_bottomRight);

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, topLeft_uv);
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, topRight_uv);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, bottomRight_uv);

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, topLeft_uv);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, bottomRight_uv);
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);
        insertVec2(m_vertexData, bottomLeft_uv);
    }
}

glm::vec3 cone::calcConeNorm(glm::vec3& pt) {
    float xNorm = (2 * pt.x);
    // float yNorm = -(0.25f) * (2.f * pt.y - 1.f);
    float yNorm =  -0.5f *(pt.y - 0.5);
    float zNorm = (2 * pt.z);

    return normalize(glm::vec3{ xNorm, yNorm, zNorm });
}

void cone::makeConeSlopeTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                             glm::vec3 topRight, glm::vec2 topRight_uv,
                             glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                             glm::vec3 bottomRight, glm::vec2 bottomRight_uv) {

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, calcConeNorm(bottomLeft));
    insertVec2(m_vertexData, bottomLeft_uv);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, calcConeNorm(topRight));
    insertVec2(m_vertexData, topRight_uv);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, calcConeNorm(bottomRight));
    insertVec2(m_vertexData, bottomRight_uv);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, calcConeNorm(bottomLeft));
    insertVec2(m_vertexData, bottomLeft_uv);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, calcConeNorm(topLeft));
    insertVec2(m_vertexData, topLeft_uv);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, calcConeNorm(topRight));
    insertVec2(m_vertexData, topRight_uv);
}

void cone::makeConeSlopeSlice(float currentTheta, float nextTheta){
    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 topLeft = glm::vec3(((i/float(m_param1)) * m_radius) * cos(currentTheta), (1.0f - (i/float(m_param1))) - m_radius , ((i/float(m_param1)) * m_radius * sin(currentTheta)));
        glm::vec2 topLeft_uv = glm::vec2(((i/float(m_param1)) * m_radius) * cos(currentTheta), ((i/float(m_param1)) * m_radius * sin(currentTheta)));

        glm::vec3 topRight = glm::vec3(((i/float(m_param1)) * m_radius) * cos(nextTheta), (1.0f - (i/float(m_param1))) - m_radius , ((i/float(m_param1)) * m_radius * sin(nextTheta)));
        glm::vec2 topRight_uv = glm::vec2(((i/float(m_param1)) * m_radius) * cos(nextTheta), ((i/float(m_param1)) * m_radius * sin(nextTheta)));

        glm::vec3 bottomLeft = glm::vec3((((i+1)/float(m_param1)) * m_radius) * cos(currentTheta), (1.0f - ((i+1)/float(m_param1))) - 0.5f , (((i+1)/float(m_param1)) * m_radius * sin(currentTheta)));
        glm::vec2 bottomLeft_uv = glm::vec2((((i+1)/float(m_param1)) * m_radius) * cos(currentTheta), (((i+1)/float(m_param1)) * m_radius * sin(currentTheta)));

        glm::vec3 bottomRight = glm::vec3((((i+1)/float(m_param1)) * m_radius) * cos(nextTheta), (1.0f - ((i+1)/float(m_param1))) - 0.5f , (((i+1)/float(m_param1)) * m_radius * sin(nextTheta)));
        glm::vec2 bottomRight_uv = glm::vec2((((i+1)/float(m_param1)) * m_radius) * cos(nextTheta), (((i+1)/float(m_param1)) * m_radius * sin(nextTheta)));
        if (i == 0) {
            bottomLeft = (bottomLeft);
            bottomRight = (bottomRight);
            glm::vec3 base_normal_bottom1 = calcConeNorm((bottomLeft));
            glm::vec3 base_normal_bottom2 = calcConeNorm((bottomRight));

            glm::vec3 tipNormal = glm::normalize(base_normal_bottom1 + base_normal_bottom2);

            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, tipNormal);
            insertVec2(m_vertexData, topRight_uv);
            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, tipNormal);
            insertVec2(m_vertexData, topLeft_uv);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, calcConeNorm(bottomRight));
            insertVec2(m_vertexData, bottomRight_uv);

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, tipNormal);
            insertVec2(m_vertexData, topLeft_uv);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, calcConeNorm(bottomRight));
            insertVec2(m_vertexData, bottomRight_uv);
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, calcConeNorm(bottomLeft));
            insertVec2(m_vertexData, bottomLeft_uv);
        } else {
            makeConeSlopeTile(topLeft, topLeft_uv, topRight, topRight_uv, bottomLeft, bottomLeft_uv, bottomRight, bottomRight_uv);
        }
    }
}

void cone::makeConeWedge(float currentTheta, float nextTheta) {
    makeConeSlopeSlice(currentTheta, nextTheta);
    makeConeCapSlice(currentTheta, nextTheta);
}

std::vector<float> cone::generateConeData(int phiTesselations, int thetaTesselations) {
    m_vertexData = std::vector<float>();
    m_radius = 0.5;
    m_param1 = phiTesselations;
    m_param2 = thetaTesselations;
    setConeVertexData();
    return m_vertexData;
}


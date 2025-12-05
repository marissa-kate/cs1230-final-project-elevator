#include "sphere.h"

sphere::sphere() {}


void sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void sphere::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}

void sphere::makeSphereTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                            glm::vec3 topRight, glm::vec2 topRight_uv,
                            glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                            glm::vec3 bottomRight, glm::vec2 bottomRight_uv) {

    insertVec3(m_SpherevertexData, topLeft);
    insertVec3(m_SpherevertexData, glm::normalize(topLeft));
    insertVec2(m_SpherevertexData, topLeft_uv);
    insertVec3(m_SpherevertexData, bottomLeft);
    insertVec3(m_SpherevertexData, glm::normalize(bottomLeft));
    insertVec2(m_SpherevertexData, bottomLeft_uv);
    insertVec3(m_SpherevertexData, bottomRight);
    insertVec3(m_SpherevertexData, glm::normalize(bottomRight));
    insertVec2(m_SpherevertexData, bottomRight_uv);

    insertVec3(m_SpherevertexData, topRight);
    insertVec3(m_SpherevertexData, glm::normalize(topRight));
    insertVec2(m_SpherevertexData, bottomLeft_uv);
    insertVec3(m_SpherevertexData, topLeft);
    insertVec3(m_SpherevertexData, glm::normalize(topLeft));
    insertVec2(m_SpherevertexData, topLeft_uv);
    insertVec3(m_SpherevertexData, bottomRight);
    insertVec3(m_SpherevertexData, glm::normalize(bottomRight));
    insertVec2(m_SpherevertexData, bottomRight_uv);

}

glm::vec2 sphere::get_uv_sides(float theta, float phi) {
    float u = 0.0f;
    float v = (phi / M_PI) + m_radius;
    if (theta < 0) {
        u = (-theta / (2.0f * M_PI));
    } else {
        u = (1.0f - (theta / (2.0f * M_PI)));
    }
    return glm::vec2(u, v);
}

void sphere::makeSphereWedge(float currentTheta, float nextTheta) {
    float radius = m_radius;
    for (int i = 0; i < m_param1; ++i) {

        float phi_curr = M_PI * ((float)i / m_param1);
        float phi_next = M_PI * ((float)(i+1) / m_param1);

        float x_topLeft = m_radius * sin(phi_curr) * cos(currentTheta);
        float y_topLeft = m_radius * cos(phi_curr);
        float z_topLeft = -m_radius * sin(phi_curr) * sin(currentTheta);

        glm::vec3 topLeft = glm::vec3(x_topLeft, y_topLeft, z_topLeft);
        glm::vec2 topLeft_uv = get_uv_sides(currentTheta, phi_curr);

        float x_topRight = m_radius * sin(phi_curr) * cos(nextTheta);
        float y_topRight = m_radius * cos(phi_curr);
        float z_topRight = -m_radius * sin(phi_curr) * sin(nextTheta);

        glm::vec3 topRight = glm::vec3(x_topRight, y_topRight, z_topRight);
        glm::vec2 topRight_uv = get_uv_sides(nextTheta, phi_curr);

        float x_bottomLeft = m_radius * sin(phi_next) * cos(currentTheta);
        float y_bottomLeft = m_radius * cos(phi_next);
        float z_bottomLeft = -m_radius * sin(phi_next) * sin(currentTheta);

        glm::vec3 bottomLeft = glm::vec3(x_bottomLeft, y_bottomLeft, z_bottomLeft);
        glm::vec2 bottomLeft_uv = get_uv_sides(currentTheta, phi_next);

        float x_bottomRight = m_radius * sin(phi_next) * cos(nextTheta);
        float y_bottomRight = m_radius * cos(phi_next);
        float z_bottomRight = -m_radius * sin(phi_next) * sin(nextTheta);

        glm::vec3 bottomRight = glm::vec3(x_bottomRight, y_bottomRight, z_bottomRight);
        glm::vec2 bottomRight_uv = get_uv_sides(nextTheta, phi_next);

        makeSphereTile(topLeft, topLeft_uv,
                       topRight, topRight_uv,
                       bottomLeft, bottomLeft_uv,
                       bottomRight, bottomRight_uv);

    }
}

void sphere::makeSphere() {
    for (int i = 0; i < m_param2; ++i) {
        float theta = (i * 2.0f * M_PI) / m_param2;
        float next_theta = ((i + 1) * 2.0f * M_PI) / m_param2;
        makeSphereWedge(theta, next_theta);
    }
}

void sphere::setSphereVertexData() {
    float thetaStep = glm::radians(360.f / m_param2);
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    makeSphereWedge(currentTheta, nextTheta);
    makeSphere();
}

std::vector<float> sphere::generateSphereData(int param1, int param2) {
    m_SpherevertexData.clear();
    m_radius = 0.5;
    m_param1 = param1;
    m_param2 = param2;
    setSphereVertexData();
    return m_SpherevertexData;
}

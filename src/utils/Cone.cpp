#include "Cone.h"
#include <cmath>
#include <algorithm> // std::max

#ifndef PI
#define PI 3.14159265358979323846f
#endif

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cone::setVertexData() {
    m_vertexData.clear();

    int p1 = std::max(3, m_param1);

    float angleStep = 2.0f * PI / p1;

    for (int i = 0; i < p1; ++i) {
        float currentAngle = i * angleStep;
        float nextAngle = (i + 1) * angleStep;
        makeWedge(currentAngle, nextAngle);
    }
}


// Inserts a glm::vec3 into a vector of floats.
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cone::makeCapTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
    // cap normal
    glm::vec3 n(0.f, -1.f, 0.f);

    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, v4);
    insertVec3(m_vertexData, n);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, n);
}

void Cone::makeCapSlice(float theta_start, float theta_end) {
    float r_outer = 0.5f;
    float y = -0.5f;
    int p2 = std::max(1, m_param2);
    float delta_r = r_outer / p2;

    for (int i = 0; i < p2; i++) {
        float r1 = i * delta_r;
        float r2 = (i + 1) * delta_r;

        glm::vec3 v1(r1 * glm::cos(theta_start), y, r1 * glm::sin(theta_start));
        glm::vec3 v2(r1 * glm::cos(theta_end), y, r1 * glm::sin(theta_end));
        glm::vec3 v3(r2 * glm::cos(theta_end), y, r2 * glm::sin(theta_end));
        glm::vec3 v4(r2 * glm::cos(theta_start), y, r2 * glm::sin(theta_start));

        makeCapTile(v1, v2, v3, v4);
    }
}

void Cone::makeSlopeTile(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4, float theta_start, float theta_end) {

    glm::vec3 n1, n2, n3, n4;
    float epsilon = 0.0001f;


    if (v1.y >= 0.5f - epsilon) {
        n1 = glm::normalize(glm::vec3(2.0f * glm::cos(theta_start), 1.0f, 2.0f * glm::sin(theta_start)));
    } else {
        float xNorm = 2.0f * v1.x;
        float yNorm = 0.25f * (1.0f - 2.0f * v1.y); // (0.25 - 0.5y)
        float zNorm = 2.0f * v1.z;
        n1 = glm::normalize(glm::vec3(xNorm, yNorm, zNorm));
    }


    if (v2.y >= 0.5f - epsilon) {
        n2 = glm::normalize(glm::vec3(2.0f * glm::cos(theta_end), 1.0f, 2.0f * glm::sin(theta_end)));
    } else {
        float xNorm = 2.0f * v2.x;
        float yNorm = 0.25f * (1.0f - 2.0f * v2.y);
        float zNorm = 2.0f * v2.z;
        n2 = glm::normalize(glm::vec3(xNorm, yNorm, zNorm));
    }

    float xNorm3 = 2.0f * v3.x;
    float yNorm3 = 0.25f * (1.0f - 2.0f * v3.y);
    float zNorm3 = 2.0f * v3.z;
    n3 = glm::normalize(glm::vec3(xNorm3, yNorm3, zNorm3));

    float xNorm4 = 2.0f * v4.x;
    float yNorm4 = 0.25f * (1.0f - 2.0f * v4.y);
    float zNorm4 = 2.0f * v4.z;
    n4 = glm::normalize(glm::vec3(xNorm4, yNorm4, zNorm4));


    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, n1);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, n3);
    insertVec3(m_vertexData, v4);
    insertVec3(m_vertexData, n4);

    insertVec3(m_vertexData, v1);
    insertVec3(m_vertexData, n1);
    insertVec3(m_vertexData, v2);
    insertVec3(m_vertexData, n2);
    insertVec3(m_vertexData, v3);
    insertVec3(m_vertexData, n3);
}

void Cone::makeSlopeSlice(float theta_start, float theta_end) {
    float totalHeight = 1.0f;
    int p2 = std::max(1, m_param2);
    float delta_y = totalHeight / p2;


    for (int i = 0; i < p2; i++) {
        float y1 = 0.5f - i * delta_y;
        float y2 = 0.5f - (i + 1) * delta_y;

        float r1 = 0.25f - 0.5f * y1;
        float r2 = 0.25f - 0.5f * y2;

        glm::vec3 v1(r1 * glm::cos(theta_start), y1, r1 * glm::sin(theta_start));
        glm::vec3 v2(r1 * glm::cos(theta_end), y1, r1 * glm::sin(theta_end));
        glm::vec3 v3(r2 * glm::cos(theta_end), y2, r2 * glm::sin(theta_end));
        glm::vec3 v4(r2 * glm::cos(theta_start), y2, r2 * glm::sin(theta_start));

        makeSlopeTile(v1, v2, v3, v4, theta_start, theta_end);
    }
}

void Cone::makeWedge(float currentAngle, float nextAngle) {
    makeCapSlice(currentAngle, nextAngle);
    makeSlopeSlice(currentAngle, nextAngle);
}

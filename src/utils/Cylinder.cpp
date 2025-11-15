#include "Cylinder.h"
#include <glm/gtc/constants.hpp>


void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = std::max(3, param1);
    m_param2 = std::max(1, param2);
    setVertexData();
}


void Cylinder::setVertexData() {
    float height = 1.0f;
    // m_radius is 0.5 in header
    float y_start = -height / 2.0f;
    float y_step = height / m_param2;
    float angle_step = (2.0f * glm::pi<float>()) / m_param1;

    // 1. Sides
    for (int j = 0; j < m_param2; ++j) { // height
        float y_curr = y_start + j * y_step;
        float y_next = y_curr + y_step;

        for (int i = 0; i < m_param1; ++i) { // slice
            float angle_curr = i * angle_step;
            float angle_next = (i + 1) * angle_step;

            // 1 slise x segment
            glm::vec3 BL_pos(m_radius * cos(angle_curr), y_curr, m_radius * sin(angle_curr));
            glm::vec3 BR_pos(m_radius * cos(angle_next), y_curr, m_radius * sin(angle_next));
            glm::vec3 TL_pos(m_radius * cos(angle_curr), y_next, m_radius * sin(angle_curr));
            glm::vec3 TR_pos(m_radius * cos(angle_next), y_next, m_radius * sin(angle_next));

            // normals
            glm::vec3 BL_norm(cos(angle_curr), 0, sin(angle_curr));
            glm::vec3 BR_norm(cos(angle_next), 0, sin(angle_next));
            glm::vec3 TL_norm(cos(angle_curr), 0, sin(angle_curr));
            glm::vec3 TR_norm(cos(angle_next), 0, sin(angle_next));

            // create the square with triangles
            insertVec3(m_vertexData, TL_pos);
            insertVec3(m_vertexData, TL_norm);
            insertVec3(m_vertexData, BR_pos);
            insertVec3(m_vertexData, BR_norm);
            insertVec3(m_vertexData, BL_pos);
            insertVec3(m_vertexData, BL_norm);

            insertVec3(m_vertexData, TL_pos);
            insertVec3(m_vertexData, TL_norm);
            insertVec3(m_vertexData, TR_pos);
            insertVec3(m_vertexData, TR_norm);
            insertVec3(m_vertexData, BR_pos);
            insertVec3(m_vertexData, BR_norm);
        }
    }

    // 2. caps
    glm::vec3 top_norm(0, 1, 0);
    glm::vec3 bot_norm(0, -1, 0);
    glm::vec3 top_center(0, height / 2.0f, 0);
    glm::vec3 bot_center(0, -height / 2.0f, 0);

    for (int i = 0; i < m_param1; ++i) {
        float angle_curr = i * angle_step;
        float angle_next = (i + 1) * angle_step;

        // top cap
        glm::vec3 v1_top(m_radius * cos(angle_curr), height / 2.0f, m_radius * sin(angle_curr));
        glm::vec3 v2_top(m_radius * cos(angle_next), height / 2.0f, m_radius * sin(angle_next));

        // triangles
        insertVec3(m_vertexData, top_center);
        insertVec3(m_vertexData, top_norm);
        insertVec3(m_vertexData, v2_top);
        insertVec3(m_vertexData, top_norm);
        insertVec3(m_vertexData, v1_top);
        insertVec3(m_vertexData, top_norm);

        // bottom cap
        glm::vec3 v1_bot(m_radius * cos(angle_curr), -height / 2.0f, m_radius * sin(angle_curr));
        glm::vec3 v2_bot(m_radius * cos(angle_next), -height / 2.0f, m_radius * sin(angle_next));

        // triangles
        insertVec3(m_vertexData, bot_center);
        insertVec3(m_vertexData, bot_norm);
        insertVec3(m_vertexData, v1_bot);
        insertVec3(m_vertexData, bot_norm);
        insertVec3(m_vertexData, v2_bot);
        insertVec3(m_vertexData, bot_norm);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

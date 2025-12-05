#include "sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    float radius = m_radius;
    for (int i = 0; i < m_param1; ++i) {

        float phi_curr = M_PI * ((float)i / m_param1);
        float phi_next = M_PI * ((float)(i+1) / m_param1);

        float x_topLeft = m_radius * sin(phi_curr) * cos(currentTheta);
        float y_topLeft = m_radius * cos(phi_curr);
        float z_topLeft = -m_radius * sin(phi_curr) * sin(currentTheta);

        glm::vec3 topLeft = glm::vec3(x_topLeft, y_topLeft, z_topLeft);

        float x_topRight = m_radius * sin(phi_curr) * cos(nextTheta);
        float y_topRight = m_radius * cos(phi_curr);
        float z_topRight = -m_radius * sin(phi_curr) * sin(nextTheta);

        glm::vec3 topRight = glm::vec3(x_topRight, y_topRight, z_topRight);

        float x_bottomLeft = m_radius * sin(phi_next) * cos(currentTheta);
        float y_bottomLeft = m_radius * cos(phi_next);
        float z_bottomLeft = -m_radius * sin(phi_next) * sin(currentTheta);

        glm::vec3 bottomLeft = glm::vec3(x_bottomLeft, y_bottomLeft, z_bottomLeft);

        float x_bottomRight = m_radius * sin(phi_next) * cos(nextTheta);
        float y_bottomRight = m_radius * cos(phi_next);
        float z_bottomRight = -m_radius * sin(phi_next) * sin(nextTheta);

        glm::vec3 bottomRight = glm::vec3(x_bottomRight, y_bottomRight, z_bottomRight);

        makeTile(topLeft, topRight, bottomLeft, bottomRight);

    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    for (int i = 0; i < m_param2; ++i) {
        float theta = (i * 2.0f * M_PI) / m_param2;
        float next_theta = ((i + 1) * 2.0f * M_PI) / m_param2;
        makeWedge(theta, next_theta);
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

    float thetaStep = glm::radians(360.f / m_param2);
    float currentTheta = 0 * thetaStep;
    float nextTheta = 1 * thetaStep;
    makeWedge(currentTheta, nextTheta);

    // Uncomment these lines to make sphere for Task 7:

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

#include "cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


// void Cone::setVertexData() {
//     // TODO for Project 5: Lights, Camera
// }
void Cone::setVertexData() {
    // Task 10: create a full Cone using the makeWedge() function you
    //          just implemented
    // Note: think about how param 2 comes into play here!
    for (int i = 0; i < m_param2; ++i) {
        float curr_theta = (float)i * (2.0f * M_PI / float(m_param2));
        float next_theta = ((float)i + 1.0f) * (2.0f * M_PI / float(m_param2));
        makeWedge(curr_theta, next_theta);
    }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}


// Task 8: create function(s) to make tiles which you can call later on
// Note: Consider your makeTile() functions from Sphere and Cube

void Cone::makeCapSlice(float currentTheta, float nextTheta){
    // Task 8: create a slice of the cap face using your
    //         make tile function(s)
    // Note: think about how param 1 comes into play here!
    glm::vec3 normal = glm::vec3(0.0f, -1.0f, 0.0f);
    for (int i = 0; i < m_param1; ++i) {

        float x_topLeft = (i + 1) / float(m_param1) * m_radius * cos(currentTheta);
        float z_topLeft = (i + 1) / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 topLeft = glm::vec3(x_topLeft, -0.5f, z_topLeft);

        float x_topRight = (i + 1) / float(m_param1) * m_radius * cos(nextTheta);
        float z_topRight = (i + 1) / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 topRight = glm::vec3(x_topRight, -0.5f, z_topRight);

        float x_bottomLeft = i / float(m_param1) * m_radius * cos(currentTheta);
        float z_bottomLeft = i / float(m_param1) * m_radius * sin(currentTheta);

        glm::vec3 bottomLeft = glm::vec3(x_bottomLeft, -0.5f, z_bottomLeft);

        float x_bottomRight = i / float(m_param1) * m_radius * cos(nextTheta);
        float z_bottomRight = i / float(m_param1) * m_radius * sin(nextTheta);

        glm::vec3 bottomRight = glm::vec3(x_bottomRight, -0.5f, z_bottomRight);

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, normal);

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, normal);
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, normal);

    }
}

glm::vec3 Cone::calcNorm(glm::vec3& pt) {
    float xNorm = (2 * pt.x);
    float yNorm = -(1.f/4.f) * (2.f * pt.y - 1.f);
    float zNorm = (2 * pt.z);

    return glm::normalize(glm::vec3{ xNorm, yNorm, zNorm });
}

void Cone::makeSlopeTile(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight) {

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

void Cone::makeSlopeSlice(float currentTheta, float nextTheta){
    // Task 9: create a single sloped face using your make
    //         tile function(s)
    // Note: think about how param 1 comes into play here!


    for (int i = 0; i < m_param1; ++i) {
        glm::vec3 topLeft = glm::vec3(((i/float(m_param1)) * m_radius) * cos(currentTheta), (1.0f - (i/float(m_param1))) - 0.5f , ((i/float(m_param1)) * m_radius * sin(currentTheta)));
        glm::vec3 topRight = glm::vec3(((i/float(m_param1)) * m_radius) * cos(nextTheta), (1.0f - (i/float(m_param1))) - 0.5f , ((i/float(m_param1)) * m_radius * sin(nextTheta)));
        glm::vec3 bottomLeft = glm::vec3((((i+1)/float(m_param1)) * m_radius) * cos(currentTheta), (1.0f - ((i+1)/float(m_param1))) - 0.5f , (((i+1)/float(m_param1)) * m_radius * sin(currentTheta)));
        glm::vec3 bottomRight = glm::vec3((((i+1)/float(m_param1)) * m_radius) * cos(nextTheta), (1.0f - ((i+1)/float(m_param1))) - 0.5f , (((i+1)/float(m_param1)) * m_radius * sin(nextTheta)));
        if (i == 0) {
            glm::vec3 base_normal_bottom1 = calcNorm(bottomLeft);
            glm::vec3 base_normal_bottom2 = calcNorm(bottomRight);

            glm::vec3 tipNormal = glm::normalize(base_normal_bottom1 + base_normal_bottom2);

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, tipNormal);
            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, tipNormal);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, calcNorm(bottomRight));

            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, tipNormal);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, calcNorm(bottomRight));
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, calcNorm(bottomLeft));
        } else {
            makeSlopeTile(topLeft, topRight, bottomLeft, bottomRight);
        }
    }

}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    // Task 10: create a single wedge of the Cone using the
    //          makeCapSlice() and makeSlopeSlice() functions you
    //          implemented in Task 5
    makeSlopeSlice(currentTheta, nextTheta);
    makeCapSlice(currentTheta, nextTheta);
}



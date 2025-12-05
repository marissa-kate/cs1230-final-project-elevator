#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec3 normal);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight,
                  glm::vec3 normal);
    void makeCapSlice(float currentTheta, float nextTheta, float y_coord, glm::vec3 normal, bool top);
    void makeWedge(float currentTheta, float nextTheta);
    void makeSlopeSlice(float currentTheta, float nextTheta);
    void makeSlopeTile(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight, glm::vec3 normal);
    glm::vec3 calcNorm(glm::vec3& pt);
    void makeCapSlice(glm::vec3 one, glm::vec3 two, glm::vec3 three, glm::vec3 four, glm::vec3 normal);

};

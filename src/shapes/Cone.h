#pragma once

#include "shapes/Primitive.h"
#include <vector>
#include <glm/glm.hpp>

class Cone: public Primitive
{
public:
    void updateParams(int param1, int param2) override;
    std::vector<float> generateShape() override {return m_vertexData;};
    int vertexCount;

private:
    glm::mat3 inertiaTensor(float m, glm::vec3 scale) override;
    void setVertexData() override;
    void makeCapSlice(float currentTheta, float nextTheta);
    void makeSlopeSlice(float currentTheta, float nextTheta);
    void makeWedge(float currentTheta, float nextTheta);
    void makeCapTile(glm::vec3 topLeft, glm::vec3 topRight,glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeSlopeTile(glm::vec3 topLeft, glm::vec3 topRight,glm::vec3 bottomLeft, glm::vec3 bottomRight);
    glm::vec3 calcNorm(glm::vec3& pt);
    float m_radius = 0.5;
    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
};

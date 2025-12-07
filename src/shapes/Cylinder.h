#pragma once

#include "shapes/Primitive.h"
#include <glm/glm.hpp>

class Cylinder : public Primitive
{
public:
    void updateParams(int param1, int param2) override;
    std::vector<float> generateShape() override {return m_vertexData;};
    int vertexCount;
private:
    glm::mat3 inertiaTensor(float m, glm::vec3 scale) override;
    void makeCapTile(glm::vec3 topLeft,glm::vec3 topRight,glm::vec3 bottomLeft,glm::vec3 bottomRight);
    void makeCapSlice(float currentTheta, float nextTheta, bool topCap);
    void makeCylinder();
    void makeSideSlice(float currentTheta, float nextTheta);
    void makeSideTile(glm::vec3 topLeft,
                                glm::vec3 topRight,
                                glm::vec3 bottomLeft,
                      glm::vec3 bottomRight);
    void setVertexData() override;
    float m_radius = 0.5;
    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    glm::vec3 getPu(glm::vec3 vertex);
};

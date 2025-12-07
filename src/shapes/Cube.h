#pragma once

#include "shapes/Primitive.h"
#include <vector>
#include <glm/glm.hpp>

class Cube : public Primitive
{
public:
    void updateParams(int param1, int param2) override;
    std::vector<float> generateShape() override {return m_vertexData;};
    int vertexCount;

private:
    glm::mat3 inertiaTensor(float m, glm::vec3 scale) override;
    void setVertexData() override;
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, int face);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, int face);
    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    glm::vec2 getUV(glm::vec3 vert, int face);
    glm::vec3 getPu(glm::vec3 vert, int face);
    glm::vec3 getPv(glm::vec3 vert, int face);
};

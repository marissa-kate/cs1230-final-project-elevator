#pragma once

#include "shapes/Primitive.h"
#include <vector>
#include <glm/glm.hpp>

class Triangle : public Primitive
{
public:
    void updateParams(int param1, int param2) override;
    std::vector<float> generateShape()override {return m_vertexData;};

private:
    void setVertexData() override;

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
};

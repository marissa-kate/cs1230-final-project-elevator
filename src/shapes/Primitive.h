#pragma once
#include <vector>
#include <glm/glm.hpp>
#define PRIMITIVE_H

class Primitive {
public:
    virtual ~Primitive() = default;
    virtual std::vector<float> generateShape() = 0;
    virtual glm::mat3 inertiaTensor(float m, glm::vec3 scale) = 0;
    virtual void updateParams(int param1, int param2) = 0;
    void insertVec3(std::vector<float> &data, glm::vec3 v) const{
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);};
    virtual void setVertexData() = 0;


};

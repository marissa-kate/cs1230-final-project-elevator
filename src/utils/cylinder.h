#ifndef CYLINDER_H
#define CYLINDER_H

#include "shapes.h"
#include <iostream>
#include <glm/glm.hpp>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

class cylinder
{
public:
    cylinder();
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);

    void setCylinderVertexData();
    void makeCylinderWedge(float currentTheta, float nextTheta);
    glm::vec3 calcCylinderNorm(glm::vec3& pt);
    void makeCylinderSlopeTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                               glm::vec3 topRight, glm::vec2 topRight_uv,
                               glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                               glm::vec3 bottomRight, glm::vec2 bottomRight_uv,
                               glm::vec3 normal);
    void makeCylinderSlopeSlice(float currentTheta, float nextTheta);
    void makeCylinderCapSlice(glm::vec3 one, glm::vec2 one_uv,
                               glm::vec3 two, glm::vec2 two_uv,
                               glm::vec3 three, glm::vec2 three_uv,
                               glm::vec3 four, glm::vec2 four_uv,
                               glm::vec3 normal);
    std::vector<float> generateCylinderData(int phiTesselations, int thetaTesselations);

    std::vector<float> m_vertexData;
    float m_radius;
    int m_param1;
    int m_param2;
};

#endif // CYLINDER_H

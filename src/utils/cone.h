#ifndef CONE_H
#define CONE_H

#include "shapes.h"
#include <iostream>
#include <glm/glm.hpp>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"


class cone
{
public:
    cone();
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);

    void setConeVertexData();
    void makeConeCapSlice(float currentTheta, float nextTheta);
    glm::vec3 calcConeNorm(glm::vec3& pt);
    void makeConeSlopeTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                           glm::vec3 topRight, glm::vec2 topRight_uv,
                           glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                           glm::vec3 bottomRight, glm::vec2 bottomRight_uv);
    void makeConeSlopeSlice(float currentTheta, float nextTheta);
    void makeConeWedge(float currentTheta, float nextTheta);
    std::vector<float> generateConeData(int phiTesselations, int thetaTesselations);

    std::vector<float> m_vertexData;
    float m_radius;
    int m_param1;
    int m_param2;
};

#endif // CONE_H

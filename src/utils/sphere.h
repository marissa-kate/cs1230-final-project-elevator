#ifndef SPHERE_H
#define SPHERE_H

#include "shapes.h"
#include <iostream>
#include <glm/glm.hpp>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

class sphere
{
public:
    sphere();
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    void makeSphereTile(glm::vec3 topLeft, glm::vec2 topLeft_uv,
                        glm::vec3 topRight, glm::vec2 topRight_uv,
                        glm::vec3 bottomLeft, glm::vec2 bottomLeft_uv,
                        glm::vec3 bottomRight, glm::vec2 bottomRight_uv);
    void makeSphereWedge(float currentTheta, float nextTheta);
    glm::vec2 get_uv_sides(float theta, float phi);
    void makeSphere();
    void setSphereVertexData();
    std::vector<float> generateSphereData2(int phiTesselations, int thetaTesselations);

    std::vector<float> generateSphereData(int phiTesselations, int thetaTesselations);

    std::vector<float> m_SpherevertexData;
    float m_radius;
    int m_param1;
    int m_param2;

};

#endif // SPHERE_H

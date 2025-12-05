#ifndef CUBE_H
#define CUBE_H
#include "shapes.h"
#include <iostream>
#include <glm/glm.hpp>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"


class cube
{
public:
    cube();
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);

    void setCubeVertexData();
    void makeCubeTile(glm::vec3 topLeft, glm::vec2 scale_topLeft_uv,
                      glm::vec3 topRight, glm::vec2 scale_topRight_uv,
                      glm::vec3 bottomLeft, glm::vec2 scale_bottomLeft_uv,
                      glm::vec3 bottomRight, glm::vec2 scale_bottomRight_uv,
                      glm::vec3 normal);
    void makeFace(glm::vec3 topLeft, glm::vec2 top_leftuv,
                  glm::vec3 topRight, glm::vec2 top_rightuv,
                  glm::vec3 bottomLeft, glm::vec2 bottom_leftuv,
                  glm::vec3 bottomRight, glm::vec2 bottom_rightuv,
                  glm::vec3 normal);
    std::vector<float> generateCubeData(int phiTesselations, int thetaTesselations);

    std::vector<float> m_CubevertexData;
    float m_radius;
    int m_param1;
    int m_param2;

};

#endif // CUBE_H

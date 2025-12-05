#ifndef SHAPES_H
#define SHAPES_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/glm.hpp>
#include "utils/sceneparser.h"
#include "utils/sphere.h"
#include "utils/cube.h"
#include "utils/cylinder.h"
#include "utils/cone.h"

class shapes
{
public:
    shapes();

    struct vao_vbo_struct {
        GLuint m_sphere_vbo;    // Stores id of VBO
        GLuint m_sphere_vao;    // Stores id of VAO
        std::vector<float> m_sphereData;

        GLuint m_cube_vbo;    // Stores id of VBO
        GLuint m_cube_vao;    // Stores id of VAO
        std::vector<float> m_cubeData;

        GLuint m_cylinder_vbo;    // Stores id of VBO
        GLuint m_cylinder_vao;    // Stores id of VAO
        std::vector<float> m_cylinderData;

        GLuint m_cone_vbo;    // Stores id of VBO
        GLuint m_cone_vao;    // Stores id of VAO
        std::vector<float> m_coneData;

        glm::mat3 m_inv_transpose_model_sphere = glm::mat3(1);
        glm::mat3 m_inv_transpose_model_cube = glm::mat3(1);
        glm::mat3 m_inv_transpose_model_cylinder = glm::mat3(1);
        glm::mat3 m_inv_transpose_model_cone = glm::mat3(1);
    };

    std::vector<float> generate_shape_data(PrimitiveType shape_type, int param1, int param2);

};

#endif // SHAPES_H

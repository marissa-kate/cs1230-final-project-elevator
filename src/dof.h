#ifndef DOF_H
#define DOF_H
#include "utils/sceneparser.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

class dof
{
public:
    dof();
    void pass_uniform(GLuint &light_shader, float thing, std::string shader_name);
    void pass_depth_uniforms(GLuint &light_shader);
    void get_z_buffer();
    inline void glErrorCheck(const char* file, int line_number) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            // Task 2: Edit this print statement to be more descriptive
            std::cout << errorNumber << "in file: " << file << " - at line: " << line_number << std::endl;

            errorNumber = glGetError();
        }
    }
    float zfar_plane;
    float znear_plane;
    float focal_length;
    float plane_in_focus;
    float aperature;
};

#endif // DOF_H

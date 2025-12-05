#include "dof.h"

#include "utils/sceneparser.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

dof::dof() {}

void dof::pass_uniform(GLuint &light_shader, float thing, std::string shader_name) {
    GLint names_loc = glGetUniformLocation(light_shader, shader_name.c_str());
    glErrorCheck(__FILE__, __LINE__);
    glUniform1f(names_loc, thing);
    glErrorCheck(__FILE__, __LINE__);
}

// void dof::get_z_buffer(int scene_width, int scene_height) {
//     int x, y;
//     for (y = 0; y < scene_height; ++y) {
//         for (x = 0; x < scene_width; ++x) {

//         }
//     }
// }

void dof::pass_depth_uniforms(GLuint &light_shader) {
    pass_uniform(light_shader, zfar_plane, "zfar_plane");
    glErrorCheck(__FILE__, __LINE__);
    pass_uniform(light_shader, znear_plane, "znear_plane");
    glErrorCheck(__FILE__, __LINE__);
    pass_uniform(light_shader, plane_in_focus, "plane_in_focus");
    glErrorCheck(__FILE__, __LINE__);
    // std::cout << "plane in focus: " <<plane_in_focus << std::endl;
    pass_uniform(light_shader, aperature, "aperature");
    glErrorCheck(__FILE__, __LINE__);
    // std::cout << "Aperature: " <<aperature << std::endl;
    pass_uniform(light_shader, focal_length, "focal_length");
    glErrorCheck(__FILE__, __LINE__);
    // float object_distance = -zfar_plane * znear_plane / (5 * (zfar_plane - znear_plane) - zfar_plane); // puts object distance in world space
    // float coc = abs((aperature * (focal_length * (object_distance - plane_in_focus))) / (object_distance * (plane_in_focus - focal_length)));
    // std::cout<< "COC: "<<glm::max(log2(coc), 1.0f) <<std::endl;
}

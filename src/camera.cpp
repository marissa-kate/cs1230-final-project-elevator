#include "camera.h"
#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

camera::camera() {}

glm::mat4 camera::getViewMatrix(SceneCameraData cameradata) {
    glm::vec3 pos = cameradata.pos;
    glm::vec3 look = cameradata.look;
    glm::vec3 up = cameradata.up;

    glm::vec4 one_t = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 two_t = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 three_t = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    glm::vec4 four_t = glm::vec4(-pos[0], -pos[1], -pos[2], 1.0f);

    glm::mat4 translate = glm::mat4(one_t, two_t, three_t, four_t);

    glm::vec3 w = -glm::normalize(look);
    glm::vec3 v = glm::normalize(up - glm::dot(up, w) * w);
    glm::vec3 u = -glm::cross(w, v);

    glm::vec4 col_1 = glm::vec4(u.x, v.x, w.x, 0.0f);
    glm::vec4 col_2 = glm::vec4(u.y, v.y, w.y, 0.0f);
    glm::vec4 col_3 = glm::vec4(u.z, v.z, w.z, 0.0f);
    glm::vec4 four_r = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 rotate(col_1, col_2, col_3, four_r);

    return rotate * translate;
}

glm::vec3 camera::rotate_axis(glm::vec3 axis, float angle, glm::vec3 camera_vector) {
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    glm::vec3 first_column = glm::vec3(cos(angle) + pow(x, 2) * (1 - cos(angle)),
                                       x * y * (1 - cos(angle)) + z * sin(angle),
                                        x * z * (1 - cos(angle) - y * sin(angle)));
    glm::vec3 second_column = glm::vec3(x * y * (1-cos(angle)) - z*sin(angle),
                                        cos(angle) + pow(y, 2) * (1 - cos(angle)),
                                        y * z * (1-cos(angle)) + x * sin(angle));
    glm::vec3 third_column = glm::vec3(x * z * (1-cos(angle)) + y*sin(angle),
                                       y * z * (1 - cos(angle)) - x * sin(angle),
                                       cos(angle) + pow(z, 2) * (1 - cos(angle)));
    glm::mat3 rod_matrix = glm::mat3(first_column, second_column, third_column);
    return rod_matrix * camera_vector;

}

glm::mat4 camera::get_proj_matrices(float height_angle, float scene_width, float scene_height,  float nearplane, float farplane, float focal_length) {

    glm::vec4 remap_z_first_col = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 remap_z_second_col = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 remap_z_third_col = glm::vec4(0.0f, 0.0f, -2.0f, 0.0f);
    glm::vec4 remap_z_fourth_col = glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);

    glm::mat4 remap_z_matrix = glm::mat4(remap_z_first_col,
                                         remap_z_second_col,
                                         remap_z_third_col,
                                         remap_z_fourth_col);

    float c = - nearplane / farplane;

    glm::vec4 mpp_first_col = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 mpp_second_col = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
    glm::vec4 mpp_third_col = glm::vec4(0.0f, 0.0f, 1.0f / (1.0f + c), -1.0f);
    glm::vec4 mpp_fourth_col= glm::vec4(0.0f, 0.0f, -c / (1.0f + c), 0.0f);

    glm::mat4 mpp_matrix = glm::mat4(mpp_first_col,
                                     mpp_second_col,
                                     mpp_third_col,
                                     mpp_fourth_col);

    float tan_value = tan(height_angle/2.0f);
    float aspect_ratio = scene_width / scene_height;        // will change also
    float theta_widt = 2.0f * atan(aspect_ratio * tan_value);

    glm::vec4 scale_view_volume_first_col = glm::vec4(1.0f / (farplane * tan(theta_widt / (2.0f * focal_length))), 0.0f, 0.0f, 0.0f);
    glm::vec4 scale_view_volume_second_col = glm::vec4(0.0f, 1.0f / (farplane * tan(height_angle / (2.0f * focal_length))), 0.0f, 0.0f);
    glm::vec4 scale_view_volume_third_col = glm::vec4(0.0f, 0.0f, 1.0f / farplane, 0.0f);
    glm::vec4 scale_view_volume_fourth_col = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 scale_view_volume = glm::mat4(scale_view_volume_first_col,
                                            scale_view_volume_second_col,
                                            scale_view_volume_third_col,
                                            scale_view_volume_fourth_col);

    return remap_z_matrix * mpp_matrix * scale_view_volume;

}

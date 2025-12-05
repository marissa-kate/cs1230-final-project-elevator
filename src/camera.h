#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "utils/sceneparser.h"

class camera
{
public:
    camera();
    glm::mat4 getViewMatrix(SceneCameraData cameradata);
    glm::mat4 get_proj_matrices(float height_angle, float scene_width, float scene_height,  float nearplane, float farplane, float focal_length);
    glm::vec3 rotate_axis(glm::vec3 axis, float angle, glm::vec3 camera_vector);
};

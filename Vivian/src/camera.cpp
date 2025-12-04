#include <stdexcept>
#include "camera.h"
#include "settings.h"
#include <cmath>

Camera::Camera() = default;

Camera::Camera(int width, int height, glm::vec4 pos, glm::vec4 look, glm::vec4 up, float heightAngle, float near, float far){

    m_width = width;
    m_height = height;
    //converting vec4 into vec3s
    m_pos = pos;
    m_look = look;
    m_up = up;

    m_heightAngle = heightAngle;
    m_widthAngle = getWidthAngle();

    updateViewMatrix();
    updateProjMatrix(near, far);

}

void Camera::updateViewMatrix(){    

    glm::vec3 w = - glm::normalize(m_look);
    glm::vec3 v = glm::normalize(m_up - glm::dot(m_up, w) * w);
    glm::vec3 u = glm::cross(v, w);


    //in column-major order
    glm::mat4 rotate (
        u.x, v.x, w.x, 0,
        u.y, v.y, w.y, 0,
        u.z, v.z, w.z, 0,
        0, 0, 0, 1);

    // glm::mat4 rotate (
    //     u.x, u.y, u.z, 0,
    //     v.x, v.y, v.z, 0,
    //     w.x, w.y, w.z, 0,
    //     0, 0, 0, 1);


    glm::mat4 translate (
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -m_pos.x, -m_pos.y, -m_pos.z, 1);

    // m_viewMatrix = rotate * translate;

    glm::vec3 pos3 = glm::vec3(m_pos);
    glm::vec3 look3 = glm::vec3(m_look);
    glm::vec3 up3 = glm::vec3(m_up);

    m_viewMatrix = glm::lookAt(pos3, pos3 + look3, up3);
}

void Camera::updateProjMatrix(float near, float far){

    float c = - near / far;

    //column major
    glm::mat4 remapping (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 1.0f);

    glm::mat4 unhinge (
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f / (1.0f + c), -1.0f,
        0.f, 0.0f, -c / (1.0f + c), 0.0f);

    float xScale = 1.0f / (far * tan(m_widthAngle / 2));
    float yScale = 1.0f / (far * tan(m_heightAngle / 2));
    float zScale = 1.0f / far;

    glm::mat4 scale (
        xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, zScale, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    m_projMatrix = remapping * unhinge * scale;

}

void Camera::translate(const glm::vec3 &delta) {
    m_pos += delta;
    updateViewMatrix();
}

void Camera::rotateX(float angle) {

    float cos = std::cos(angle);
    float sin = std::sin(angle);

    glm::mat3 rodrigues (
        cos, 0, - sin,
        0, cos + (1.0f - cos), 0,
        sin, 0, cos
    );

    m_look = glm::normalize(rodrigues * m_look);
    // m_up = glm::normalize(rodrigues * glm::vec4(m_up, 0.0f));

    updateViewMatrix();
}

void Camera::rotateY(float angle) {

    glm::vec3 right = glm::normalize(glm::cross(m_look, m_up));

    float cos = std::cos(angle);
    float sin = std::sin(angle);

    glm::mat3 rodrigues (
        cos + right.x * right.x * (1.0f - cos), right.x * right.y * (1.0f - cos) + right.z * sin, right.x * right.z * (1.0f - cos) - right.y * sin,
        right.x * right.y * (1.0f - cos) - right.z * sin, cos + right.y * right.y * (1.0f - cos), right.y * right.z * (1.0f - cos) + right.x * sin,
        right.x * right.z * (1.0f - cos) + right.y * sin, right.y * right.z * (1.0f - cos) - right.x * sin, cos + right.z * right.z * (1.0f - cos)
        );


    m_look = glm::normalize(rodrigues * m_look);

    m_up = glm::normalize(glm::cross(right, m_look));

    updateViewMatrix();

}





glm::mat4 Camera::getProjMatrix() const {
    return m_projMatrix;
}


glm::mat4 Camera::getViewMatrix() const {

    return m_viewMatrix;
}

float Camera::getAspectRatio() const {
    //ratio taken from inis directly, haven't figured out how to generalize
    return static_cast<float>(m_width)/m_height;
}

float Camera::getHeightAngle() const {
    return m_heightAngle;
}

float Camera::getWidthAngle() const {
    return 2.0f * atan(tan(m_heightAngle/2.0f) * getAspectRatio());

}

float Camera::getFocalLength() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

float Camera::getAperture() const {
    // Optional TODO: implement the getter or make your own design
    throw std::runtime_error("not implemented");
}

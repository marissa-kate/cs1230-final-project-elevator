#include <QtCore/qnamespace.h>
#include <iostream>
#include <stdexcept>
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"

void Camera::init(SceneCameraData cameraData){
    heightAngle=cameraData.heightAngle;
    pos=cameraData.pos;
    look = cameraData.look;
    up = cameraData.up;
    calculateViewMatrix();
}

void Camera::calculateViewMatrix(){
    // Task 8: return the camera view matrix
    glm::mat4 mTranslate = glm::mat4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        -pos.x, -pos.y, -pos.z, 1.f
        );

    glm::vec3 w = -glm::normalize(look);
    glm::vec3 v = glm::normalize(up-glm::dot(up,w)*w);
    glm::vec3 u = glm::cross(v,w);
    glm::mat4 mRotate = glm::mat4(
        u.x, v.x, w.x, 0.f,
        u.y, v.y, w.y, 0.f,
        u.z, v.z, w.z, 0.f,
        0.f, 0.f, 0.f, 1.f
        );
    viewMatrix = mRotate*mTranslate;
    inverseMatrix = glm::inverse(viewMatrix);
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return inverseMatrix;
}

void Camera::calculatePerspectiveMatrix(float near, float far, float heightAngle, float aspectR) {
    // heightAngle is the vertical field of view (in radians)

    float tan_half_thetah = tan(heightAngle / 2.0f);
    float tan_half_thetaw = aspectR * tan_half_thetah;

    // Step 1: Scale from camera space to the view volume (unnormalized)
    glm::mat4 scale = glm::mat4(
        1.f / (far * (tan_half_thetaw)), 0, 0, 0,
        0, 1.f / (far * (tan_half_thetah)), 0, 0,
        0, 0, 1.f / far, 0,
        0, 0, 0, 1.f);
    // Step 2: Unhinge matrix (maps z from [near, far] to [near/far, 1])
    float c = - near / far;
    glm::mat4 unhinge = glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1.f / (1.f + c), -1.f,
        0, 0, -c / (1.f + c), 0);
    // Step 3: OpenGL depth remap (maps z to [-1, 1])
    glm::mat4 glRemap = glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -2, 0,
        0, 0, -1, 1
        );

    projectionMatrix = glRemap * unhinge * scale;
}


glm::mat4 Camera::getPerspectiveMatrix() const {
    return projectionMatrix;
}


float Camera::getAspectRatio(float sceneWidth, float sceneHeight) const {
    return sceneWidth/sceneHeight;
}

float Camera::getHeightAngle() const {
    return heightAngle;
}

void Camera::translateCamera(Qt::Key keyPressed, float deltaTime){
    glm::vec3 dir(0.0f);
    const float speed = 5.0f;
    switch(keyPressed){
    case 0x57: //W
        dir = glm::normalize(look);
        break;
    case 0x53: //s
        dir = glm::normalize(-look);
        break;
    case 0x41: //A
        dir = -glm::normalize(glm::cross(look, up));
        break;
    case 0x44: //D
        dir = glm::normalize(glm::cross(look, up));
        break;
    case 0x20: //space
        dir = glm::normalize(glm::vec3(0, 1.f, 0));
        break;
    case 0x01000021: //command
        dir = glm::normalize(glm::vec3(0, -1.f, 0));
        break;
    default:
        break;
    }
    glm::vec3 displacement = speed * deltaTime * dir;
    pos += displacement;
    calculateViewMatrix();

}

void Camera::rotateCamera(int deltaX, int deltaY){
    const float sensitivity = 0.0025f; // radians per pixel
    float yaw   = -deltaX * sensitivity; // left-right
    float pitch = -deltaY * sensitivity;
    const glm::vec3 worldUp = glm::vec3(0, 1.f, 0);
    look = rotateAroundAxis(look, worldUp, yaw);
    up = rotateAroundAxis(up, worldUp, yaw);
    glm::vec3 right = glm::normalize(glm::cross(look, up));
    look = rotateAroundAxis(look, right, pitch);
    up   = glm::normalize(glm::cross(right, look));
    calculateViewMatrix();
}

glm::vec3 Camera::rotateAroundAxis(const glm::vec3 &v, const glm::vec3 &axis, float angle)  {
    float cosA = cos(angle);
    float sinA = sin(angle);
    return v * cosA + glm::cross(axis, v) * sinA + axis * glm::dot(axis, v) * (1 - cosA);
}

// float Camera::getFocalLength() const {
//     // Optional TODO: implement the getter or make your own design
//     throw std::runtime_error("not implemented");
// }

// float Camera::getAperture() const {
//     // Optional TODO: implement the getter or make your own design
//     throw std::runtime_error("not implemented");
// }

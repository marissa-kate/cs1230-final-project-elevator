#pragma once

#include "utils/scenedata.h"
#include <QtCore/qnamespace.h>
#include <glm/glm.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:
    void init(SceneCameraData cameraData);
    void calculateViewMatrix();
    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    glm::mat4 getPerspectiveMatrix() const;
    glm::mat4 getProjMatrix(float heightAngle, float aspectR, float near, float far);

    glm::mat4 getInverseViewMatrix() const;

    void calculatePerspectiveMatrix(float near, float far, float heightAngle, float aspectR);

    // Returns the aspect ratio of the camera.
    float getAspectRatio(float sceneWidth, float sceneHeight) const;
    void translateCamera(Qt::Key keyPressed, float deltaTime);
    void rotateCamera(int deltaX, int deltaY);
    static glm::vec3 rotateAroundAxis(const glm::vec3 &v, const glm::vec3 &axis, float angle);

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;
    glm::vec3 pos;
    glm::vec3 look;
    glm::vec3 up;

private:
    glm::mat4 viewMatrix;
    glm::mat4 inverseMatrix;
    glm::mat4 projectionMatrix;
    float heightAngle;


};

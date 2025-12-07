#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// A class representing a virtual camera.

// Feel free to make your own design choices for Camera class, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class Camera {
public:

    int m_width;
    int m_height;

    glm::vec3 m_pos;
    glm::vec3 m_look;
    glm::vec3 m_up;

    glm::mat4 m_viewMatrix;
    glm::mat4 m_projMatrix;

    float m_heightAngle;
    float m_widthAngle;



    Camera();
    //constructor
    Camera(int width, int height, glm::vec4 pos, glm::vec4 look, glm::vec4 up, float heightAngle, float near, float far);

    void updateViewMatrix();
    void updateProjMatrix(float near, float far);

    void translate(const glm::vec3 &delta);
    void rotateX(float angle);
    void rotateY(float angle);



    // Returns the view matrix for the current camera settings.
    // You might also want to define another function that return the inverse of the view matrix.
    glm::mat4 getViewMatrix() const;

    // Returns current projection matrix with current near and farplane setttngs
    glm::mat4 getProjMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    // Returns the width angle of the camera in RADIANS.
    float getWidthAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;
};

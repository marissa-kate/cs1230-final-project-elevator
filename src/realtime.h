#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include "utils/scenedata.h"
#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "utils/sceneparser.h"
#include "utils/Cone.h"
#include "utils/Cylinder.h"
#include "utils/Cube.h"
#include "utils/Sphere.h"
#include "utils/Tet.h"
#include "utils/Triangle.h"
#include "utils/sceneparser.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    bool m_gl_initialized = false;
    double m_devicePixelRatio;

    // For triangle
    GLuint m_shader_program;

    // use primitive type as key to get VBO or VAO
    std::map<PrimitiveType, GLuint> m_vbo_map;
    std::map<PrimitiveType, GLuint> m_vao_map;
    //TODO: This is a dummy rn
    std::vector<RenderShapeData> m_shapes_to_draw;

    //for making projection matrix
    glm::mat4 makePerspectiveMatrix(float fovY, float aspect, float near, float far);
    glm::mat4 makeViewMatrix(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);
    glm::mat4 makeRotationMatrix(float angle, const glm::vec3& axis);

    //save instance of primitive class as member
    std::map<PrimitiveType, GLuint> m_vertex_count_map;
    Triangle m_triangle;
    Cube m_cube;
    Sphere m_sphere;
    Cylinder m_cylinder;
    Cone m_cone;
    void setupShapeVAO(PrimitiveType type);
    void updateAllShapeTessellations();

    //parser
    SceneParser m_parser;
    RenderData m_renderData;
};

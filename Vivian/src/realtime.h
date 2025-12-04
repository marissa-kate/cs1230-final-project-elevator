#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "camera.h"
#include "utils/sceneparser.h"


class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);
    void bindTexture();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes


private:
    void paintTexture(GLuint texture, bool filter);
    void makeFBO();
    void paintBackground(GLuint bg);

    GLuint m_shader; // Stores id of shader program
    GLuint m_fullscreen_shader;
    GLuint m_background_shader;


    QImage m_texture_image;
    GLuint m_texture;
    int m_filter_type;

    QImage m_background_image;
    GLuint m_background;

    // GLuint m_texture_shader;
    // GLuint m_fullscreen_vbo;
    // GLuint m_fullscreen_vao;
    // QImage m_image;
    // GLuint m_kitten_texture;


    // GLuint m_vbo;    // Stores id of VBO
    // GLuint m_vao;    // Stores id of VAO

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
    double m_devicePixelRatio;

    Camera m_camera;
    glm::mat4 m_proj = glm::mat4(1.0f);
    glm::mat4 m_view = glm::mat4(1.0f);

    RenderData metaData;

    float ka; // Ambient term
    float kd; // Diffuse term
    float ks; // Specular term

    void bindVBOVAO(PrimitiveType primType, std::vector<GLfloat> vertices);
    void makeShape(GLuint vbo, GLuint vao, int vertexCount, const glm::mat4 &model, const SceneMaterial &material);


    //VBO/VAO Shapes (Primitives!)

    GLuint m_sphere_vao;
    GLuint m_sphere_vbo;
    int m_sphere_vertices;

    GLuint m_cone_vao;
    GLuint m_cone_vbo;
    int m_cone_vertices;


    GLuint m_cube_vao;
    GLuint m_cube_vbo;
    int m_cube_vertices;


    GLuint m_cylinder_vao;
    GLuint m_cylinder_vbo;
    int m_cylinder_vertices;



    // struct GLShape {
    //     GLuint vao;
    //     GLuint vbo;
    //     int vertexCount;
    //     std::vector<GLfloat> vertices;
    //     glm::mat4 model_matrix = glm::mat4(1.0f);
    // };

    // std::vector<GLShape> m_shapes;


    GLuint m_fullscreen_vao;
    GLuint m_fullscreen_vbo;

    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;

    GLuint m_defaultFBO;
    int m_screen_width;
    int m_screen_height;
    int m_fbo_width;
    int m_fbo_height;

};



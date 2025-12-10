#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "inputs.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

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
    Inputs inputs;
    float aspect;
    // in Realtime.h
    bool m_pendingSettingsUpdate = false;
    bool m_pendingScene = true;


    //VAO, VBO, FBO, RBO
    GLuint m_cube_vbo = 0, m_cone_vbo = 0, m_cyl_vbo = 0, m_sphere_vbo = 0;
    GLuint m_cube_vao = 0, m_cone_vao = 0, m_cyl_vao = 0, m_sphere_vao = 0;
    int m_cube_vertexCount = 0;
    int m_sphere_vertexCount = 0;
    int m_cone_vertexCount = 0;
    int m_cyl_vertexCount = 0;


    GLuint m_defaultFBO;
    GLuint m_fbo;
    GLuint m_composite_fbo;
    GLuint colorBuffers[2];
    GLuint m_hdr_fbo;
    int m_fbo_width;
    int m_fbo_height;
    GLuint m_fbo_texture;

    GLuint position_fbo;
    GLuint normal_fbo;
    GLuint ambinet_shininess_texture;
    GLuint albedo_spec_texture;
    GLuint second_depth_fbo;
    GLuint final_color;
    GLuint composite_color;
    GLuint first_depth_fbo;
    GLuint blur_texture;
    GLuint color_grading_texture;

    float zfar_plane;
    float znear_plane;
    float aperature;
    float focal_length;
    float plane_in_focus;

    GLuint m_rbo;
    GLuint m_fullscreen_vao;
    GLuint pingpong_fbo[2];
    GLuint pingpong_colorBuffers[2];
    GLuint m_fullscreen_vbo;
    GLuint m_bright_texture;
    GLuint m_obj_texture;
    QImage m_obj_texture_image;

    void drawScenePrimitives();
    void makeFBO();
    void setupVBO(GLuint vao, GLuint vbo, PrimitiveType type);
    void paintTexture();
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void blurBrightTexture();
    void drawPrimitives();
    void drawLights();
    void final_color_pass();
    void store_texture(GLuint &thing, int m_fbo_width, int m_fbo_height, const GLuint &internalFormat, const GLenum &format, const GLenum &type,
                      const GLenum &first_parameter_value, const GLenum &second_parameter_value, const GLenum &attachment);
    void pass_texture_sampler2D_uniforms(GLuint &dof_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id);
    void pass_dof_sampler2D_uniforms(GLuint &dof_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id);
    void pass_dof_uniform(GLuint &dof_shader, float thing, std::string shader_name);
    void depth_of_field_pass();
    inline void glErrorCheck(const char* file, int line_number) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            // Task 2: Edit this print statement to be more descriptive
            std::cout << errorNumber << "in file: " << file << " - at line: " << line_number << std::endl;

            errorNumber = glGetError();
        }
    }
    GLuint m_geometry_shader;
    GLuint m_phong_shader;
    GLuint m_blur_shader;
    GLuint m_composite_shader;

    GLuint m_dof_shader;

    //color-grading
    GLuint m_color_shader;
    GLuint m_2d_lut;
    int m_lut_size;
    std::vector<float> m_lut_data;
    void loadCubeLUT(const QString& path);

    void paintTexture(GLuint texture, bool filter);

    glm::mat4 m_proj;
    glm::vec4 camera_pos; //world space camera
    Camera cam;
    glm::mat4 m_view;

    double n_bodies;

    std::vector<float> m_bufferData;
    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames
    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;
    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;
};

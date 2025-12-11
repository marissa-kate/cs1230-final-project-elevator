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
#include "audiocapture.h"
#include "particlesystem.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);
    void bindObjectTexture();

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
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
    void bloomBlurPasses();
    void applyPhysics(double t0, double t1, double deltaTime);
    void drawPrimitives();
    void drawLights();
    void final_color_pass();
    void store_texture(GLuint &thing, int m_fbo_width, int m_fbo_height, const GLuint &internalFormat, const GLenum &format, const GLenum &type,
                       const GLenum &first_parameter_value, const GLenum &second_parameter_value, const GLenum &attachment);
    void pass_texture_sampler2D_uniforms(GLuint &dof_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id);
    void pass_dof_sampler2D_uniforms(GLuint &dof_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id);
    void pass_dof_uniform(GLuint &dof_shader, float thing, std::string shader_name);
    void depth_of_field_pass();

    Inputs inputs;

    bool m_pendingSettingsUpdate = false;
    bool m_pendingScene = true;


    //VAO, VBO
    GLuint m_cube_vbo = 0, m_cone_vbo = 0, m_cyl_vbo = 0, m_sphere_vbo = 0;
    GLuint m_cube_vao = 0, m_cone_vao = 0, m_cyl_vao = 0, m_sphere_vao = 0;
    GLuint m_fullscreen_vao;
    GLuint m_fullscreen_vbo;
    int m_cube_vertexCount = 0;
    int m_sphere_vertexCount = 0;
    int m_cone_vertexCount = 0;
    int m_cyl_vertexCount = 0;


    GLuint m_defaultFBO,
        m_fbo,
        m_composite_fbo,
        second_depth_fbo,
        first_depth_fbo,
        position_fbo,
        normal_fbo,
        pingpong_fbo[2];

    //textures
    GLuint m_composite_fbo_texture,
        m_fbo_texture,
        first_particle_texture,
        second_particle_texture,
        m_bright_texture,
        color_grading_texture,
        m_obj_texture,
        albedo_spec_texture,
        ambinet_shininess_texture;
    GLuint colorBuffers[2], pingpong_colorBuffers[2];

    GLuint m_rbo;
    QImage m_obj_texture_image;



    GLuint final_color;
    GLuint composite_color;

    //Camera
    glm::mat4 m_proj;
    glm::vec4 camera_pos; //world space camera
    Camera cam;
    glm::mat4 m_view;

    GLuint m_dof_shader;
    float zfar_plane;
    float aspect;
    float znear_plane;
    float aperature;
    float focal_length;
    float plane_in_focus;
    int m_fbo_width;
    int m_fbo_height;

    //Shapes!
    GLuint m_geometry_shader;
    GLuint m_phong_shader;
    GLuint m_blur_shader;
    GLuint m_composite_shader;

    void paramUpdate();
    double n_bodies;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    std::vector<float> m_bufferData;

    //color-grading
    GLuint m_color_shader;
    GLuint m_2d_lut;
    int m_lut_size;
    int lut_index;
    std::vector<float> m_lut_data;
    void loadCubeLUT(const QString& path);
    void paintTexture(GLuint texture, bool filter);


    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames


    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    bool keyJustPressed(Qt::Key key,
                        const std::unordered_map<Qt::Key, bool>& prev);
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not
    std::unordered_map<Qt::Key, bool> m_prevKeyMap;

    // Device Correction Variables
    double m_devicePixelRatio;
    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;

    //particle
    std::vector<std::unique_ptr<ParticleSystem>> m_particleSystems;
    GLuint m_particle_shader;
    float m_simTime = 0.0f;

    // audio
    AudioCapture* m_audioCapture = nullptr;

    inline void glErrorCheck(const char* file, int line_number) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            // Task 2: Edit this print statement to be more descriptive
            std::cout << errorNumber << "in file: " << file << " - at line: " << line_number << std::endl;

            errorNumber = glGetError();
        }
    }
};

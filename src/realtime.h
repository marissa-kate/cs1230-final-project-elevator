#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
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
#include "shapes.h"
#include "utils/sceneparser.h"
#include "settings.h"
#include "camera.h"
#include "realtimehelper.h"
#include "dof.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

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

    // float m_zoomZ = 1.0;

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    inline void glErrorCheck(const char* file, int line_number) {
        GLenum errorNumber = glGetError();
        while (errorNumber != GL_NO_ERROR) {
            // Task 2: Edit this print statement to be more descriptive
            std::cout << errorNumber << "in file: " << file << " - at line: " << line_number << std::endl;

            errorNumber = glGetError();
        }
    }

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    double m_devicePixelRatio;

    std::vector<GLuint> shape_vaos;                     // 0 = sphere, 1 = cube, 2 = cylinder, 3 = cone
    GLuint sphere_vaos;
    GLuint cube_vaos;
    GLuint cylinder_vaos;
    GLuint cone_vaos;
    std::vector<GLuint> shape_vbos;
    GLuint sphere_vbos;
    GLuint cube_vbos;
    GLuint cylinder_vbos;
    GLuint cone_vbos;
    std::vector<std::vector<float>> shape_datas;
    std::vector<float> sphere_datas;
    std::vector<float> cube_datas;
    std::vector<float> cylinder_datas;
    std::vector<float> cone_datas;

    float speed;

    camera camera_data;

    SceneParser sceneparser;
    shapes shape;
    RenderData realtime_renderdata;

    std::vector<glm::mat3> inv_transpose_models_shapes;

    GLuint m_shader; // Stores id of shader program
    GLuint light_shader;
    GLuint geometry_shader_2;
    GLuint final_shader;

    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view  = glm::mat4(1);
    glm::mat4 m_proj  = glm::mat4(1);

    glm::mat4 getViewMatrix(SceneCameraData cameradata);
    std::vector<std::vector<float>> get_shape_data();
    std::array<SceneLightData, 8> make_vector_arrays(std::vector<SceneLightData> light_data);
    void bind_vaos_vbos(int index);
    void clear_everything();
    void bind_fullscreen_quad(GLuint &shader);
    void pass_final_uniforms();
    realtimehelper helper;

    glm::mat4 mvp_matrix = glm::mat4(1);
    glm::vec4 world_camera_pos = glm::vec4(1);

    glm::mat4 m_view_proj = glm::mat4(1);

    glm::vec4 m_lightPos; // The world-space position of the point light

    GLuint m_light_vertex;
    GLuint m_light_frag;

    Settings settings_variable;
    int param1;
    int param2;

    float distance_from_obj;

    GLuint fbo;
    GLuint light_fbo;
    GLuint geo2_fbo;
    GLuint fbo_renderbuffer;
    GLuint position_fbo;
    GLuint normal_fbo;
    GLuint ambient_shininess_texture;
    GLuint albedo_spec_texture;
    GLuint light_col_texture;
    GLuint depth_fbo;
    GLuint m_default_fbo;
    GLuint first_depth_fbo;
    GLuint second_depth_fbo;
    GLuint third_depth_fbo;
    std::vector<GLuint> first_geometry_textures;
    std::vector<GLuint> second_light_textures;

    GLuint fullscreen_vbo;
    GLuint fullscreen_vao;

    float scene_width;
    float scene_height;
    float fbo_width;
    float fbo_height;
    float tan_valu;
    float height_angl;

    dof depth_of_field_class;

    float aspect_rati;
    float theta_width;

    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    QPoint m_prevMousePos;
    float  m_angleX;
    float  m_angleY;
    float  m_zoom;

    /**
     * @brief verifyVAO - prints in the terminal how OpenGL would interpret `triangleData` using the inputted VAO arguments
     * @param triangleData - the vector containing the triangle data
     * @param index - same as glVertexAttribPointer()
     * @param size - same as glVertexAttribPointer()
     * @param stride - same as glVertexAttribPointer()
     * @param offset - same as glVertexAttribPointer()
     */
    void verifyVAO(std::vector<GLfloat> &triangleData, GLuint index, GLsizei size, GLsizei stride, const void* offset) {

        int newStride = int(stride / 4);
        int groupNum = 0;
        int newOffset = static_cast<int>(reinterpret_cast<intptr_t>(offset)) / 4;

        for (int i = newOffset; i < triangleData.size(); i = i + newStride) {
            std::cout << "Group " << groupNum << " of Values for VAO index " << index << std::endl;
            std::cout << "[";
            for (auto j = i; j < i + size; ++j) {
                if (j != i + size - 1) {
                    std::cout << triangleData[j]<< ", ";
                } else {
                    std::cout << triangleData[j]<< "]" << std::endl;
                }
            }
            groupNum = groupNum + 1;
        }
        std::cout << "" << std::endl;
    }

    // // from lab 8:
    // // VAO/VBO
    // QOpenGLVertexArrayObject m_vao;
    // QOpenGLBuffer m_vbo;

    // // Shape shader program stuff
    // QOpenGLShaderProgram *m_program = nullptr;
    // int m_default_projLoc;
    // int m_default_mvLoc;
    // int m_default_normalLoc;
    // int m_default_lightPos;

    // // Normal arrows shader program stuff
    // QOpenGLShaderProgram *m_normalsProgram = nullptr; // arrow body
    // int m_normal_projLoc;
    // int m_normal_mvLoc;
    // QOpenGLShaderProgram *m_normalsTipsProgram = nullptr; // arrow head
    // int m_normalTip_projLoc;
    // int m_normalTip_mvLoc;

    // // Wireframe shader program stuff
    // QOpenGLShaderProgram *m_wireframeProgram = nullptr;
    // int m_wireframe_projLoc;
    // int m_wireframe_mvLoc;

    // // Vertices, matrices, etc.
    // std::vector<GLfloat> verts;
    // int m_numTriangles;
    // glm::mat4x4 m_proj   = glm::mat4(1.0f);
    // glm::mat4x4 m_camera = glm::mat4(1.0f);
    // glm::mat4x4 m_world  = glm::mat4(1.0f);
    // static bool m_transparent;

    // // Tracking shape to render
    // int m_currShape;
    // Triangle* m_triangle;
    // Cube* m_cube;
    // Sphere* m_sphere;
    // Cylinder* m_cylinder;
    // Cone* m_cone;
    // // Tetrahedron* m_tet;

    // // Tracking params
    // int m_currParam1;
    // int m_currParam2;
    // bool m_currShowWireframeNormals = true;


    // // from lab 8:
    // void bindVbo();

};

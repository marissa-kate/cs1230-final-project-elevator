#include "realtime.h"
#include "utils/sceneparser.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include "shapes.h"
#include "dof.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "realtimehelper.h"


Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    makeCurrent();
    glDeleteProgram(m_shader);
    glDeleteProgram(light_shader);
    glDeleteProgram(final_shader);
    glDeleteVertexArrays(1, &sphere_vaos);
    doneCurrent();
    glDeleteBuffers(1, &sphere_vbos);
    doneCurrent();
    glDeleteBuffers(1, &fullscreen_vbo);
    doneCurrent();
    glDeleteVertexArrays(1, &cube_vaos);
    doneCurrent();
    glDeleteBuffers(1, &cube_vbos);
    doneCurrent();
    glDeleteVertexArrays(1, &cylinder_vaos);
    doneCurrent();
    glDeleteBuffers(1, &cylinder_vbos);
    doneCurrent();
    glDeleteVertexArrays(1, &cone_vaos);
    doneCurrent();
    glDeleteBuffers(1, &cone_vbos);
    doneCurrent();
    glDeleteVertexArrays(1, &fullscreen_vao);
    doneCurrent();

    glDeleteTextures(1, &position_fbo);
    glDeleteTextures(1, &normal_fbo);
    glDeleteTextures(1, &ambient_shininess_texture);
    glDeleteTextures(1, &albedo_spec_texture);
    glDeleteTextures(1, &second_depth_fbo);
    glDeleteTextures(1, &first_depth_fbo);
    glDeleteTextures(1, &light_col_texture);
    glDeleteRenderbuffers(1, &fbo_renderbuffer);
    glDeleteFramebuffers(1, &fbo);
}

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

void Realtime::initializeGL() {
    makeCurrent();
    SceneParser sceneparser;

    RenderData renderData = RenderData();

    realtime_renderdata = renderData;

    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    glClearColor(0, 0, 0, 1.0);
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    light_shader = ShaderLoader::createShaderProgram(":/resources/shaders/light.vert", ":/resources/shaders/light.frag");
    final_shader = ShaderLoader::createShaderProgram(":/resources/shaders/final_light.vert", ":/resources/shaders/final_light.frag");
    m_default_fbo = 2;
    sceneChanged();
    settingsChanged();
}

std::vector<std::vector<float>> Realtime::get_shape_data() {
    std::vector<std::vector<float>> empty_data(4);
    for (int i = 0; i < realtime_renderdata.shape_types.size(); ++i) {

        PrimitiveType current_shape_type = realtime_renderdata.shape_types[i];
        std::vector<float> data = shape.generate_shape_data(current_shape_type, param1, param2);

        if (current_shape_type == PrimitiveType::PRIMITIVE_SPHERE) {
            empty_data[0] = data;
            sphere_datas = data;
        } else if (current_shape_type == PrimitiveType::PRIMITIVE_CUBE) {
            empty_data[1] = data;
            cube_datas = data;
        } else if (current_shape_type == PrimitiveType::PRIMITIVE_CYLINDER) {
            empty_data[2] = data;
            cylinder_datas = data;
        } else if (current_shape_type == PrimitiveType::PRIMITIVE_CONE) {
            empty_data[3] = data;
            cone_datas = data;
        }
    }
    return empty_data;
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here

    // ================ First Geometry Pass (store normals, shininess, and depth ================//

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glErrorCheck(__FILE__, __LINE__);

    glEnable(GL_DEPTH_TEST);
    glErrorCheck(__FILE__, __LINE__);
    glDepthMask(GL_TRUE);
    glErrorCheck(__FILE__, __LINE__);

    glViewport(0, 0, fbo_width, fbo_height);
    glErrorCheck(__FILE__, __LINE__);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glErrorCheck(__FILE__, __LINE__);

    helper.pass_pre_lighting_uniforms(m_shader, realtime_renderdata, shape_vaos, shape_datas, m_view_proj, m_view, first_depth_fbo, scene_width, scene_height);


    // ================ Second Lighting Pass (store light-only diffuse and specular) ================//

    helper.pass_uniform_lights(realtime_renderdata, depth_of_field_class, light_shader, position_fbo, normal_fbo,
                               ambient_shininess_texture, albedo_spec_texture, first_depth_fbo, m_view, scene_width, scene_height, fullscreen_vao,
                               light_col_texture, m_model, m_proj);

    glBindTexture(GL_TEXTURE_2D, light_col_texture); //generate mipmaps
    glErrorCheck(__FILE__, __LINE__);
    glGenerateMipmap(GL_TEXTURE_2D);
    glErrorCheck(__FILE__, __LINE__);
    glBindTexture(GL_TEXTURE_2D, 0);
    glErrorCheck(__FILE__, __LINE__);

    // ================ Third (Final) DOF Pass (output final color ================//

    glBindFramebuffer(GL_FRAMEBUFFER, m_default_fbo);
    glErrorCheck(__FILE__, __LINE__);

    glViewport(0, 0, scene_width, scene_height);
    glErrorCheck(__FILE__, __LINE__);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glErrorCheck(__FILE__, __LINE__);

    helper.pass_final_uniforms(final_shader, fullscreen_vao, depth_of_field_class, m_view,
                               m_proj, scene_width, scene_height, light_col_texture, second_depth_fbo, normal_fbo);

}

void Realtime::resizeGL(int w, int h) {
    glDeleteTextures(1, &position_fbo);
    glDeleteTextures(1, &normal_fbo);
    glDeleteTextures(1, &ambient_shininess_texture);
    glDeleteTextures(1, &albedo_spec_texture);
    glDeleteTextures(1, &first_depth_fbo);
    glDeleteRenderbuffers(1, &fbo_renderbuffer);
    glDeleteFramebuffers(1, &fbo);

    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    scene_width = size().width() * m_devicePixelRatio;
    scene_height = size().height() * m_devicePixelRatio;
    fbo_width = scene_width;
    fbo_height = scene_height;

    helper.bind_geometry_textures(fbo, fbo_width, fbo_height, position_fbo, normal_fbo, ambient_shininess_texture, albedo_spec_texture, light_col_texture,  m_default_fbo, first_depth_fbo, second_depth_fbo);

}

void Realtime::bind_vaos_vbos(int index) {
    makeCurrent();
    glGenBuffers(1, &shape_vbos[index]);
    glErrorCheck(__FILE__, __LINE__);
    glBindBuffer(GL_ARRAY_BUFFER, shape_vbos[index]);
    glErrorCheck(__FILE__, __LINE__);
    glBufferData(GL_ARRAY_BUFFER,shape_datas[index].size() * sizeof(GLfloat),shape_datas[index].data(), GL_STATIC_DRAW);
    glErrorCheck(__FILE__, __LINE__);
    glGenVertexArrays(1, &shape_vaos[index]);
    glErrorCheck(__FILE__, __LINE__);
    glBindVertexArray(shape_vaos[index]);
    glErrorCheck(__FILE__, __LINE__);
}

void Realtime::clear_everything() {
    shape_vaos.clear();
    shape_vbos.clear();
    shape_datas.clear();
    sphere_vbos = 0;
    sphere_vaos = 0;
    sphere_datas.clear();
    realtime_renderdata = RenderData();
    sceneparser = SceneParser();
    shape = shapes();
    helper = realtimehelper();
    depth_of_field_class = dof();

    fullscreen_vao = 0;
    fullscreen_vbo = 0;

    aspect_rati = 0;
    theta_width = 0;
}

void Realtime::bind_fullscreen_quad(GLuint &shader) {
    // glUseProgram(shader);
    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS  and UVs  //
         -1.0f,  1.0f, 0.0f,
         0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f,
         0.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, 1.0f,
         -1.0f,  1.0f, 0.0f,
         0.0f, 1.0f,
         1.0f, -1.0f, 0.0f,
         1.0f, 0.0f,
         };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &fullscreen_vbo);
    glErrorCheck(__FILE__, __LINE__);
    glBindBuffer(GL_ARRAY_BUFFER, fullscreen_vbo);
    glErrorCheck(__FILE__, __LINE__);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glErrorCheck(__FILE__, __LINE__);
    glGenVertexArrays(1, &fullscreen_vao);
    glErrorCheck(__FILE__, __LINE__);
    glBindVertexArray(fullscreen_vao);
    glErrorCheck(__FILE__, __LINE__);

    int position_index = 0;
    glEnableVertexAttribArray(position_index);
    glErrorCheck(__FILE__, __LINE__);
    glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glErrorCheck(__FILE__, __LINE__);

    int uv_index = 1;
    glEnableVertexAttribArray(uv_index);
    glErrorCheck(__FILE__, __LINE__);
    glVertexAttribPointer(uv_index, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glErrorCheck(__FILE__, __LINE__);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glErrorCheck(__FILE__, __LINE__);
    glBindVertexArray(0);
    glErrorCheck(__FILE__, __LINE__);
    // glUseProgram(0);
}

void Realtime::sceneChanged() {
    makeCurrent();

    clear_everything();

    SceneCameraData cameraData = realtime_renderdata.cameraData;
    sceneparser.parse(settings.sceneFilePath, realtime_renderdata);
    m_view = camera_data.getViewMatrix(cameraData);
    m_proj = /*glm::perspective(realtime_renderdata.cameraData.heightAngle, scene_width/scene_height, settings.nearPlane, settings.farPlane);*/
camera_data.get_proj_matrices(realtime_renderdata.cameraData.heightAngle, scene_width, scene_height,
                                           settings.nearPlane, settings.farPlane, settings.focal_length);
    m_view_proj = m_proj * m_view;

    depth_of_field_class.zfar_plane = settings.farPlane;
    depth_of_field_class.znear_plane = settings.nearPlane;
    depth_of_field_class.aperature = settings.aperature;
    depth_of_field_class.focal_length = settings.focal_length;
    depth_of_field_class.plane_in_focus = settings.focal_plane;

    param1 = fmax(settings.shapeParameter1, 1);
    param2 = fmax(settings.shapeParameter2, 3);

    shape_vbos.resize(4);
    shape_vaos.resize(4);

    shape_datas = get_shape_data();     // populate shape data

    for (int i = 0; i < realtime_renderdata.shapes.size(); ++i) {

        if (realtime_renderdata.shapes[i].primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
            // std::cout << "sphere vbo id: " << shape_vbos[0] << std::endl;
            bind_vaos_vbos(0);
        } else if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_CUBE) {
            bind_vaos_vbos(1);
        } else if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_CYLINDER) {
            bind_vaos_vbos(2);
        } else if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_CONE) {
            bind_vaos_vbos(3);
        }
        // Enable and define attribute 0 to store vertex positions
        int position_index = 0;
        glEnableVertexAttribArray(position_index);
        glErrorCheck(__FILE__, __LINE__);
        glVertexAttribPointer(position_index,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(0));
        glErrorCheck(__FILE__, __LINE__);

        int normal_index = 1;
        glEnableVertexAttribArray(normal_index);
        glErrorCheck(__FILE__, __LINE__);
        glVertexAttribPointer(normal_index,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glErrorCheck(__FILE__, __LINE__);

        int uv_index = 2;
        glEnableVertexAttribArray(uv_index);
        glErrorCheck(__FILE__, __LINE__);
        glVertexAttribPointer(uv_index,2,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void*>(6 * sizeof(GLfloat)));
        glErrorCheck(__FILE__, __LINE__);

        // verifyVAO(m_sphereData, position_index, 2, 4, reinterpret_cast<void*>(0));

        // Task 14: Unbind your VBO and VAO here
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glErrorCheck(__FILE__, __LINE__);
        glBindVertexArray(0);
        glErrorCheck(__FILE__, __LINE__);

    }

    bind_fullscreen_quad(light_shader);

    unsigned int pos_fbo, norm_fbo, diff_bo, spec_fbo, first_depth, second_depth, light_color;

    position_fbo = pos_fbo;
    normal_fbo = norm_fbo;
    ambient_shininess_texture = diff_bo;
    albedo_spec_texture = spec_fbo;
    first_depth_fbo = first_depth;
    light_col_texture = light_color;
    second_depth_fbo = second_depth;

    helper.bind_geometry_textures(fbo, fbo_width, fbo_height, position_fbo, normal_fbo, ambient_shininess_texture, albedo_spec_texture, light_col_texture,  m_default_fbo, first_depth_fbo, second_depth_fbo);

    update();
    // asks for a PaintGL() call to occur
    glErrorCheck(__FILE__, __LINE__);
}

void Realtime::settingsChanged() {
    param1 = fmax(settings.shapeParameter1, 1);
    param2 = fmax(settings.shapeParameter2, 3);

    depth_of_field_class.zfar_plane = settings.farPlane;
    depth_of_field_class.znear_plane = settings.nearPlane;
    depth_of_field_class.aperature = settings.aperature;
    depth_of_field_class.focal_length = settings.focal_length;
    depth_of_field_class.plane_in_focus = settings.focal_plane;

    shape_datas = get_shape_data();

    m_proj = camera_data.get_proj_matrices(realtime_renderdata.cameraData.heightAngle, scene_width, scene_height, settings.nearPlane, settings.farPlane, settings.focal_length);
    m_view_proj = m_proj * m_view;

    for (int i = 0; i < realtime_renderdata.shapes.size() ; ++i) {
        if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_SPHERE) {
            bind_vaos_vbos(0);
        } else if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_CUBE) {
            bind_vaos_vbos(1);
        } else if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_CYLINDER) {
            bind_vaos_vbos(2);
        } else if (realtime_renderdata.shapes[i].primitive.type  == PrimitiveType::PRIMITIVE_CONE) {
            bind_vaos_vbos(3);
        }
        int position_index = 0;
        glEnableVertexAttribArray(position_index);
        glVertexAttribPointer(position_index,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void *>(0));

        int normal_index = 1;
        glEnableVertexAttribArray(normal_index);
        glVertexAttribPointer(normal_index,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        int uv_index = 2;
        glEnableVertexAttribArray(uv_index);
        glVertexAttribPointer(uv_index,2,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat),reinterpret_cast<void*>(6 * sizeof(GLfloat)));

        // Task 14: Unbind your VBO and VAO here
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    update();

}

// ================== Camera Movement!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        glm::vec3 up3 = glm::vec3(realtime_renderdata.cameraData.up);
        glm::vec3 look3 = glm::vec3(realtime_renderdata.cameraData.look);
        if (deltaX != 0) {
            float speed_x = 0.02f * deltaX ;
            up3 = glm::normalize(camera_data.rotate_axis(glm::vec3(0.0f, 1.0f, 0.0f), speed_x, up3));
            look3 = glm::normalize(camera_data.rotate_axis(glm::vec3(0.0f, 1.0f, 0.0f), speed_x, look3));

        }
        if (deltaY != 0) {
            float speed_y = 0.02f * deltaY ;
            glm::vec3 perpendicular_axis = glm::normalize(glm::vec3(look3.y * up3.z - look3.z * up3.y,
                                                                    look3.z * up3.x - look3.x * up3.z,
                                                                    look3.x * up3.y - look3.y * up3.x));
            up3 = glm::normalize(camera_data.rotate_axis(perpendicular_axis, speed_y, up3));
            look3 = glm::normalize(camera_data.rotate_axis(perpendicular_axis, speed_y, look3));

            look3 = glm::normalize(look3);
            up3 = glm::normalize(up3 - glm::dot(up3, look3) * look3);    // accoutns for floating point accumulation errors to fix perpendicularity for axises.
            realtime_renderdata.cameraData.look = glm::normalize(glm::vec4(look3, 0.0f));
            realtime_renderdata.cameraData.up   = glm::normalize(glm::vec4(up3, 0.0f));
        }

        m_view = camera_data.getViewMatrix(realtime_renderdata.cameraData);
        m_view_proj = m_proj * m_view;

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    float speed = 5.0f * deltaTime;
    glm::vec4 forward = normalize(realtime_renderdata.cameraData.look);
    glm::vec4 up = normalize(realtime_renderdata.cameraData.up);
    glm::vec4 right   = glm::vec4(
        forward.y * up.z - forward.z * up.y,
        forward.z * up.x - forward.x * up.z,
        forward.x * up.y - forward.y * up.x,
        0.0f
        );
    if (m_keyMap[Qt::Key_W]) {
        realtime_renderdata.cameraData.pos += forward * speed;
    }
    if (m_keyMap[Qt::Key_S]) {
        realtime_renderdata.cameraData.pos -= forward * speed;
    }
    if (m_keyMap[Qt::Key_D]) {
        realtime_renderdata.cameraData.pos += right * speed;
    }
    if (m_keyMap[Qt::Key_A]) {
        realtime_renderdata.cameraData.pos -= right * speed;
    }
    if (m_keyMap[Qt::Key_Space]) {
        realtime_renderdata.cameraData.pos += up * speed;
    }
    if (m_keyMap[Qt::Key_Control]) {
        realtime_renderdata.cameraData.pos -= up * speed;
    }

    m_view = camera_data.getViewMatrix(realtime_renderdata.cameraData);
    m_view_proj = m_proj * m_view;

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}

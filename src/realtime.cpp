#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <glm/glm.hpp>
#include <iostream>
#include "utils/shaderloader.h"
#include "settings.h"
#include <glm/gtc/quaternion.hpp>
#include "utils/phys.h"

std::vector<RigidBody> Bodies;
// ================== Rendering the Scene!

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
    m_keyMap[Qt::Key_Up]       = false;
    m_keyMap[Qt::Key_Down]       = false;
    m_keyMap[Qt::Key_Left]       = false;
    m_keyMap[Qt::Key_Right]       = false;
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    makeCurrent();
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteBuffers(1, &m_cone_vbo);
    glDeleteBuffers(1, &m_cyl_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);
    glDeleteVertexArrays(1, &m_cone_vao);
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteVertexArrays(1, &m_cyl_vao);
    glDeleteTextures(1, &m_obj_texture);

    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteTextures(1, &m_bright_texture);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteTextures(1, &first_depth_fbo);
    glDeleteTextures(1, &second_depth_fbo);
    glDeleteTextures(1, &final_color);
    glDeleteTextures(1, &position_fbo);
    glDeleteTextures(1, &normal_fbo);
    glDeleteTextures(1, &ambinet_shininess_texture);
    glDeleteTextures(1, &albedo_spec_texture);
    glDeleteRenderbuffers(1, &m_rbo);
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteProgram(m_geometry_shader);
    glDeleteProgram(m_phong_shader);
    glDeleteProgram(m_dof_shader);
    glDeleteProgram(m_blur_shader);
    glDeleteProgram(m_composite_shader);
    glDeleteProgram(m_phong_shader);
    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();
    m_defaultFBO = 5;

    // Initializing GL
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
    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    //Initialize camera
    cam.calculateViewMatrix();
    aspect = (float)(size().width() * m_devicePixelRatio)
             / (float)(size().height() * m_devicePixelRatio);
    cam.calculatePerspectiveMatrix((float)settings.nearPlane, (float) settings.farPlane, cam.getHeightAngle(),
                                   aspect);
    camera_pos = glm::vec4(cam.pos, 1.0);

    m_blur_shader = ShaderLoader::createShaderProgram("resources/shaders/texture.vert", "resources/shaders/texture.frag");
    m_geometry_shader = ShaderLoader::createShaderProgram("resources/shaders/default.vert", "resources/shaders/default.frag");
    m_phong_shader = ShaderLoader::createShaderProgram("resources/shaders/light.vert", "resources/shaders/light.frag");
    m_composite_shader = ShaderLoader::createShaderProgram("resources/shaders/final.vert", "resources/shaders/final.frag");
    m_dof_shader = ShaderLoader::createShaderProgram("resources/shaders/dof.vert", "resources/shaders/dof.frag");
    m_color_shader = ShaderLoader::createShaderProgram("resources/shaders/color-grading.vert", "resources/shaders/color-grading.frag");

    //initialize vaos and vbos
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glGenBuffers(1, &m_cyl_vbo);
    glGenBuffers(1, &m_sphere_vbo);
    glGenBuffers(1, &m_cone_vbo);
    glGenBuffers(1, &m_cube_vbo);
    glGenVertexArrays(1, &m_sphere_vao);
    glGenVertexArrays(1, &m_cone_vao);
    glGenVertexArrays(1, &m_cube_vao);
    glGenVertexArrays(1, &m_cyl_vao);
    setupVBO(m_cone_vao, m_cone_vbo, PrimitiveType::PRIMITIVE_CONE);
    setupVBO(m_cube_vao, m_cube_vbo, PrimitiveType::PRIMITIVE_CUBE);
    setupVBO(m_cyl_vao, m_cyl_vbo, PrimitiveType::PRIMITIVE_CYLINDER);
    setupVBO(m_sphere_vao, m_sphere_vbo, PrimitiveType::PRIMITIVE_SPHERE);

    bindTexture();

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.f,  1.f, 0.0f, 0.f, 1.f,
            -1.f, -1.f, 0.0f, 0.f, 0.f,
            1.f, -1.f, 0.0f, 1.f, 0.f,

            -1.f,  1.f, 0.f, 0.f, 1.f,
            1.f, -1.f, 0.f, 1.f, 0.f,
            1.f,  1.f, 0.f, 1.f, 1.f
        };

    //Fullscreen quad for depth buffer
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    makeFBO();

    //set color-grading uniform for texture
    glUseProgram(m_color_shader);
    glUniform1i(glGetUniformLocation(m_color_shader, "u_texture"), 0);
    glUseProgram(0);

    //load lut
    QString filepath = "resources/70 CGC LUTs/Look LUTs/Grand Budapest.cube";

    loadCubeLUT(filepath);

    glGenTextures(1, &m_2d_lut);

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_3D, m_2d_lut);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, m_lut_size, m_lut_size, m_lut_size, 0, GL_RGB, GL_FLOAT, m_lut_data.data());

    // Task 6: Set min and mag filters' interpolation mode to linear
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Task 7: Unbind kitten texture
    // glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, 1);

    // Task 10: Set the texture.frag uniform for our texture
    glUseProgram(m_color_shader);
    glUniform1i(glGetUniformLocation(m_color_shader, "lut"), 1);
    glUseProgram(0);

}
void Realtime::loadCubeLUT(const QString& path) {

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("#") || line.isEmpty())
            continue;

        if (line.startsWith("LUT_3D_SIZE")) {
            m_lut_size = line.split(" ")[1].toInt();
            m_lut_data.reserve(m_lut_size * m_lut_size * m_lut_size * 3);
            continue;
        }

        // Parse RGB triple
        QStringList vals = line.split(" ");
        if (vals.size() == 3) {
            m_lut_data.push_back(vals[0].toFloat());
            m_lut_data.push_back(vals[1].toFloat());
            m_lut_data.push_back(vals[2].toFloat());

        }
    }
}

void Realtime::store_texture(GLuint &thing, int m_fbo_width, int m_fbo_height, const GLuint &internalFormat, const GLenum &format, const GLenum &type,
                            const GLenum &first_parameter_value, const GLenum &second_parameter_value, const GLenum &attachment) {
    glGenTextures(1, &thing);
    glBindTexture(GL_TEXTURE_2D, thing);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_fbo_width, m_fbo_height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, first_parameter_value);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, second_parameter_value);
    glBindTexture(GL_TEXTURE_2D, 0);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, thing, 0);
}

// for next time, i need to add the final output in texture, color grading, and final

/**
 * @brief Realtime::makeFBO
 */
void Realtime::makeFBO(){
        // Main scene FBO & depth texture

        glGenTextures(1, &m_fbo_texture);
        glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGBA16F*/GL_RGBA32F, m_fbo_width, /*m_fbo_width*/m_fbo_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //blur texture
        glGenTextures(1, &m_bright_texture);
        glBindTexture(GL_TEXTURE_2D, m_bright_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGBA16F*/GL_RGBA32F, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // position texture
        store_texture(position_fbo, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT2);
        // normal texture
        store_texture(normal_fbo, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT3);
        // diffuse texture
        store_texture(ambinet_shininess_texture, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT4);
        // specular texture
        store_texture(albedo_spec_texture, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT5);
        // final color texture
        // store_texture(final_color, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT7);
        // second depth texture
        store_texture(second_depth_fbo, m_fbo_width, m_fbo_height, GL_R32F, GL_RED, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT6);

        // new texture for blur
        // store_texture(blur_texture, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT8);
        // // new texture for color grading
        // store_texture(color_grading_texture, m_fbo_width, m_fbo_height, GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_COLOR_ATTACHMENT9);

        // depth buffer
        store_texture(first_depth_fbo, m_fbo_width, m_fbo_height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_DEPTH_ATTACHMENT);

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // glGenTextures(1, &final_color);
        // glBindTexture(GL_TEXTURE_2D, final_color);
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        //renderbuffer
        glGenRenderbuffers(1, &m_rbo);      // check this is sus for depth
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);

        GLuint attachments[8] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6/*, GL_COLOR_ATTACHMENT7 *//*, GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9*/};
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_bright_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, position_fbo, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, normal_fbo, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, ambinet_shininess_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, albedo_spec_texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, second_depth_fbo, 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, final_color, 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT8, GL_TEXTURE_2D, blur_texture, 0);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT9, GL_TEXTURE_2D, color_grading_texture, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

        glDrawBuffers(8, attachments);

        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

        // ---------- Ping-pong FBOs for blur ----------
        glGenFramebuffers(2, pingpong_fbo);
        glGenTextures(2, pingpong_colorBuffers);

        for(int i = 0; i < 2; i++){
            glBindFramebuffer(GL_FRAMEBUFFER, pingpong_fbo[i]);
            glBindTexture(GL_TEXTURE_2D, pingpong_colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpong_colorBuffers[i], 0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

        // ---------- Composite FBOs for DOF ----------
        glGenFramebuffers(1, &m_composite_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_composite_fbo);

        glGenTextures(1, &composite_color);
        glBindTexture(GL_TEXTURE_2D, composite_color);
        glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGBA16F*/GL_RGBA32F, m_fbo_width, /*m_fbo_width*/m_fbo_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenTextures(1, &final_color);
        glBindTexture(GL_TEXTURE_2D, final_color);
        glTexImage2D(GL_TEXTURE_2D, 0, /*GL_RGBA16F*/GL_RGBA32F, m_fbo_width, /*m_fbo_width*/m_fbo_height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        GLuint attachments2[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, composite_color, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, final_color, 0);

        glDrawBuffers(2, attachments2);

        glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    }

/**
 * @brief Realtime::setupVBO a helper method to generate VBO data and VAO attributes for a primitive
 * @param vao
 * @param vbo
 * @param type
 */
void Realtime::setupVBO(GLuint vao, GLuint vbo, PrimitiveType type) {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    m_bufferData = inputs.generateVBO(type, settings.shapeParameter1, settings.shapeParameter2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_bufferData.size(), m_bufferData.data(), GL_STATIC_DRAW);

    // Each vertex has 6 floats: position (3) + normal (3)
    int vertexCount = m_bufferData.size() / 14;
    // Record vertex count by type
    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE:      m_cube_vertexCount = vertexCount; break;
    case PrimitiveType::PRIMITIVE_SPHERE:    m_sphere_vertexCount = vertexCount; break;
    case PrimitiveType::PRIMITIVE_CONE:      m_cone_vertexCount = vertexCount; break;
    case PrimitiveType::PRIMITIVE_CYLINDER:  m_cyl_vertexCount = vertexCount; break;
    default: break;
    }
    // Enable attribute pointers here
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14* sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //uvs
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

    //Pu
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(8 * sizeof(GLfloat)));

    //Pv
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(11 * sizeof(GLfloat)));

}

void Realtime::pass_dof_sampler2D_uniforms(GLuint &dof_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id) {
    glActiveTexture(texture_slot);
    glBindTexture(GL_TEXTURE_2D, fbo_thing);
    GLint location_name = glGetUniformLocation(dof_shader, shader_name.c_str());
    glUniform1i(location_name, id);
}

void Realtime::pass_dof_uniform(GLuint &dof_shader, float thing, std::string shader_name) {
    GLint names_loc = glGetUniformLocation(dof_shader, shader_name.c_str());
    glUniform1f(names_loc, thing);
}

void Realtime::depth_of_field_pass() {
    glUseProgram(m_dof_shader);
    glBindVertexArray(m_fullscreen_vao);

    pass_dof_sampler2D_uniforms(m_dof_shader, normal_fbo, GL_TEXTURE3, "normal", 3);
    glErrorCheck(__FILE__,__LINE__);
    pass_dof_sampler2D_uniforms(m_dof_shader, final_color, GL_TEXTURE7, "light_final_col", 7);
    glErrorCheck(__FILE__,__LINE__);
    pass_dof_sampler2D_uniforms(m_dof_shader, second_depth_fbo, GL_TEXTURE6, "depth", 6);
    glErrorCheck(__FILE__,__LINE__);

    pass_dof_uniform(m_dof_shader, zfar_plane, "zfar_plane");
    glErrorCheck(__FILE__,__LINE__);
    pass_dof_uniform(m_dof_shader, znear_plane, "znear_plane");
    glErrorCheck(__FILE__,__LINE__);
    pass_dof_uniform(m_dof_shader, plane_in_focus, "plane_in_focus");
    pass_dof_uniform(m_dof_shader, aperature, "aperature");
    pass_dof_uniform(m_dof_shader, focal_length, "focal_length");

    GLint sreen_space_resolution_loc = glGetUniformLocation(m_dof_shader, "screen_space_resolution");
    glUniform2fv(sreen_space_resolution_loc, 1, &glm::vec2(m_fbo_width, m_fbo_height)[0]);

    GLint world_cam_pos_loc = glGetUniformLocation(m_dof_shader, "world_cam_pos");
    glm::vec4 world_camera_pos = inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniform4fv(world_cam_pos_loc, 1, &world_camera_pos[0]);

    GLint proj_matrix_loc = glGetUniformLocation(m_dof_shader, "proj_matrix");
    glUniformMatrix4fv(proj_matrix_loc, 1, GL_FALSE, &m_proj[0][0]);

    GLint view_matrix_loc = glGetUniformLocation(m_dof_shader, "m_view");
    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, &m_view[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glErrorCheck(__FILE__,__LINE__);
    glBindVertexArray(0);
    glErrorCheck(__FILE__,__LINE__);
    glUseProgram(0);
    glErrorCheck(__FILE__,__LINE__);
}

void Realtime::paintGL() {
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    cam.calculatePerspectiveMatrix(settings.nearPlane, settings.farPlane, cam.getHeightAngle(),  aspect);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glViewport(0, 0, m_fbo_width, m_fbo_height);
    glErrorCheck(__FILE__, __LINE__);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glErrorCheck(__FILE__, __LINE__);

    // geometry pass (without lights)
    drawPrimitives();
    glErrorCheck(__FILE__,__LINE__);

    // phong pass
    drawLights();
    glErrorCheck(__FILE__,__LINE__);

    //blur pass
    blurBrightTexture();
    glErrorCheck(__FILE__,__LINE__);

    glBindFramebuffer(GL_FRAMEBUFFER, m_composite_fbo);

    glClear(GL_COLOR_BUFFER_BIT);
    glErrorCheck(__FILE__, __LINE__);

    // Final rendering pass
    glUseProgram(m_composite_shader);

    glErrorCheck(__FILE__,__LINE__);
    glUniform1i(glGetUniformLocation(m_composite_shader, "scene"), 0);
    glErrorCheck(__FILE__,__LINE__);
    glUniform1i(glGetUniformLocation(m_composite_shader, "bloomBlur"), 1);
    glErrorCheck(__FILE__,__LINE__);
    glUniform1f(glGetUniformLocation(m_composite_shader, "exposure"), settings.exposure);
    glErrorCheck(__FILE__,__LINE__);

    glActiveTexture(GL_TEXTURE0);
    glErrorCheck(__FILE__,__LINE__);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture); // original scene
    glErrorCheck(__FILE__,__LINE__);
    glActiveTexture(GL_TEXTURE1);
    glErrorCheck(__FILE__,__LINE__);
    glBindTexture(GL_TEXTURE_2D, pingpong_colorBuffers[0]); // final blurred bloom
    glErrorCheck(__FILE__,__LINE__);
    glBindVertexArray(m_fullscreen_vao);
    glErrorCheck(__FILE__,__LINE__);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glErrorCheck(__FILE__,__LINE__);
    glBindVertexArray(0);

    //final color grading pass:

    paintTexture(composite_color, true);

    //depth of field pass


    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    glBindTexture(GL_TEXTURE_2D, final_color); //generate mipmaps
    glErrorCheck(__FILE__,__LINE__);
    glGenerateMipmap(GL_TEXTURE_2D);
    glErrorCheck(__FILE__,__LINE__);
    glBindTexture(GL_TEXTURE_2D, 0);
    glErrorCheck(__FILE__,__LINE__);

    depth_of_field_pass();

}

void Realtime::paintTexture(GLuint texture, bool filter){
    glUseProgram(m_color_shader);

    // Set bool
    glUniform1i(glGetUniformLocation(m_color_shader, "bool_texture"), filter);

    //set lut size
    glUniform1i(glGetUniformLocation(m_color_shader, "lut_size"), m_lut_size);


    // ================================
    // 1. MAIN IMAGE → texture unit 0
    // ================================
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(m_color_shader, "u_texture"), 8);

    // ================================
    // 2. 2D LUT → texture unit 1
    // ================================
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_3D, m_2d_lut);
    glUniform1i(glGetUniformLocation(m_color_shader, "lut"), 9);

    glBindVertexArray(m_fullscreen_vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::bindTexture(){
    if (settings.hasTexture){
        // QString filepath = QString("/Users/vhcch/Desktop/Vivian/brown/cs1230/project-6-final-project-gear-up-vhchen29/scenefiles/bump-mapping/topleft.png");

        m_obj_texture_image = settings.bumpTexture;

        // Task 2: Format image to fit OpenGL
        // m_texture_image = m_texture_image.convertToFormat(QImage::Format_Grayscale8).mirrored();

        // Task 3: Generate texture
        glGenTextures(1, &m_obj_texture);

        // Task 9: Set the active texture slot to texture slot 0
        glActiveTexture(GL_TEXTURE0);

        // Task 4: Bind texture
        glBindTexture(GL_TEXTURE_2D, m_obj_texture);

        // Task 5: Load image into  texture
        glTexImage2D(GL_TEXTURE_2D,0, GL_RED, m_obj_texture_image.width(), m_obj_texture_image.height(), 0, GL_RED, GL_UNSIGNED_BYTE,
                     m_obj_texture_image.bits());

        // Task 6: Set min and mag filters' interpolation mode to linear
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Task 7: Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(m_geometry_shader);
        GLint samplerLoc = glGetUniformLocation(m_geometry_shader, "u_texture");
        std::cout << "u_texture location = " << samplerLoc << std::endl;
        glUniform1i(samplerLoc, 0);
        glUseProgram(m_geometry_shader);
        glUniform1i(glGetUniformLocation(m_geometry_shader, "hasTexture"), settings.hasTexture);
        glUniform1i(glGetUniformLocation(m_geometry_shader, "bump_depth"), settings.bumpDepth);
        glUseProgram(0);
    }

    glUseProgram(m_geometry_shader);
    glUniform1i(glGetUniformLocation(m_geometry_shader, "hasTexture"), settings.hasTexture);
    glUniform1i(glGetUniformLocation(m_geometry_shader, "bump_depth"), settings.bumpDepth);
    glUseProgram(0);

}

void Realtime::drawPrimitives(){
    //Draw geometry
    glUseProgram(m_geometry_shader);

    m_view = cam.getViewMatrix();
    glErrorCheck(__FILE__,__LINE__);
    m_proj = cam.getPerspectiveMatrix();
    glErrorCheck(__FILE__,__LINE__);

    glUniformMatrix4fv(glGetUniformLocation(m_geometry_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
    glErrorCheck(__FILE__,__LINE__);
    glUniformMatrix4fv(glGetUniformLocation(m_geometry_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);
    glErrorCheck(__FILE__,__LINE__);

    glUniform1i(glGetUniformLocation(m_geometry_shader, "bump_depth"), settings.bumpDepth);
    glErrorCheck(__FILE__,__LINE__);

    if (settings.hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glErrorCheck(__FILE__,__LINE__);
        glBindTexture(GL_TEXTURE_2D, m_obj_texture);
        glErrorCheck(__FILE__,__LINE__);
    } /*else {
        glDisable(GL_TEXTURE_2D);
        glErrorCheck(__FILE__,__LINE__);
    }*/

    std::vector<RenderShapeData> shapes = inputs.getPrimitives();
    for(RenderShapeData &shape: shapes){
        glUniformMatrix4fv(glGetUniformLocation(m_geometry_shader, "m_model"), 1, GL_FALSE, &shape.ctm[0][0]);
        glErrorCheck(__FILE__,__LINE__);
        glUniform1f(glGetUniformLocation(m_geometry_shader, "m_shininess"), fmax(shape.primitive.material.shininess, 1.0));
        glErrorCheck(__FILE__,__LINE__);
        glUniform4fv(glGetUniformLocation(m_geometry_shader, "cAmbient"), 1, &shape.primitive.material.cAmbient[0]);
        glErrorCheck(__FILE__,__LINE__);
        glUniform4fv(glGetUniformLocation(m_geometry_shader, "cDiffuse"), 1, &shape.primitive.material.cDiffuse[0]);
        glErrorCheck(__FILE__,__LINE__);
        glUniform4fv(glGetUniformLocation(m_geometry_shader, "cSpecular"), 1, &shape.primitive.material.cSpecular[0]);
        glErrorCheck(__FILE__,__LINE__);

        GLuint vaoAddress, vboAddress, vertexCount = 0;
        switch(shape.primitive.type){
        case PrimitiveType::PRIMITIVE_CUBE:
            vboAddress = m_cube_vbo;
            vaoAddress = m_cube_vao;
            vertexCount = m_cube_vertexCount;
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            vboAddress = m_cone_vbo;
            vaoAddress = m_cone_vao;
            vertexCount = m_cone_vertexCount;
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            vboAddress = m_cyl_vbo;
            vaoAddress = m_cyl_vao;
            vertexCount = m_cyl_vertexCount;
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            vboAddress = m_sphere_vbo;
            vaoAddress = m_sphere_vao;
            vertexCount = m_sphere_vertexCount;
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
        }
        glBindBuffer(GL_ARRAY_BUFFER, vboAddress);
        glErrorCheck(__FILE__,__LINE__);
        glBindVertexArray(vaoAddress);
        glErrorCheck(__FILE__,__LINE__);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glErrorCheck(__FILE__,__LINE__);
    }
}

void Realtime::pass_texture_sampler2D_uniforms(GLuint &light_shader, GLuint &fbo_thing, const GLenum &texture_slot, std::string shader_name,const GLint &id) {
    glActiveTexture(texture_slot);
    glBindTexture(GL_TEXTURE_2D, fbo_thing);
    GLint location_name = glGetUniformLocation(light_shader, shader_name.c_str());
    glUniform1i(location_name, id);
}

void Realtime::drawLights(){
    glUseProgram(m_phong_shader);
    glBindVertexArray(m_fullscreen_vao);

    glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);

    pass_texture_sampler2D_uniforms(m_phong_shader, position_fbo, GL_TEXTURE2, "color_position", 2);
    pass_texture_sampler2D_uniforms(m_phong_shader, normal_fbo, GL_TEXTURE3, "color_normal", 3);
    pass_texture_sampler2D_uniforms(m_phong_shader, ambinet_shininess_texture, GL_TEXTURE4, "ambient_shininess", 4);
    pass_texture_sampler2D_uniforms(m_phong_shader, albedo_spec_texture, GL_TEXTURE5, "albedo_spec", 5);
    pass_texture_sampler2D_uniforms(m_phong_shader, second_depth_fbo, GL_TEXTURE6, "depth", 6);
    // pass_texture_sampler2D_uniforms(m_phong_shader, final_color, GL_TEXTURE7, "final_color", 7);

    glUniform4fv(glGetUniformLocation(m_phong_shader, "camera_pos"), 1, &camera_pos[0]);

    inputs.sendLightUniformData(m_phong_shader);
    inputs.sendGlobalUniformData(m_phong_shader);

    glUniform1f(glGetUniformLocation(m_phong_shader, "fog_minDist"), 5.0f);
    glUniform1f(glGetUniformLocation(m_phong_shader, "fog_maxDist"), 25.0f);

    glm::vec4 fogColor = glm::vec4(0, 0, 0, 1.0f);
    glUniform4fv(glGetUniformLocation(m_phong_shader, "fog_color"), 1, &fogColor[0]);

    glUniform1f(glGetUniformLocation(m_phong_shader, "bloomThreshold"), settings.bloomThreshold);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

// void Realtime::drawPrimitives(){
//     glUseProgram(m_phong_shader);
//     m_view = cam.getViewMatrix();
//     m_proj = cam.getPerspectiveMatrix();
//     glUniform1i(glGetUniformLocation(m_phong_shader, "bump_depth"), settings.bumpDepth);

//     if (settings.hasTexture) {
//         glActiveTexture(GL_TEXTURE0);
//         glBindTexture(GL_TEXTURE_2D, m_obj_texture);
//     } else {
//         glDisable(GL_TEXTURE_2D);
//     }

//     glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
//     glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);
//     glUniform4fv(glGetUniformLocation(m_phong_shader, "camera_pos"), 1, &camera_pos[0]);

//     inputs.sendLightUniformData(m_phong_shader);
//     inputs.sendGlobalUniformData(m_phong_shader);

//     glUniform1f(glGetUniformLocation(m_phong_shader, "fog_minDist"), 5.0f);
//     glUniform1f(glGetUniformLocation(m_phong_shader, "fog_maxDist"), 25.0f);

//     glm::vec4 fogColor = glm::vec4(0, 0, 0, 1.0f);
//     glUniform4fv(glGetUniformLocation(m_phong_shader, "fog_color"), 1, &fogColor[0]);

//     glUniform1f(glGetUniformLocation(m_phong_shader, "bloomThreshold"), settings.bloomThreshold);

//     //Draw geometry
//     std::vector<RenderShapeData> shapes = inputs.getPrimitives();
//     for(RenderShapeData &shape: shapes){
//         glUniformMatrix4fv(glGetUniformLocation(m_phong_shader, "m_model"), 1, GL_FALSE, &shape.ctm[0][0]);
//         glUniform1f(glGetUniformLocation(m_phong_shader, "m_shininess"), fmax(shape.primitive.material.shininess, 1.0));
//         glUniform4fv(glGetUniformLocation(m_phong_shader, "cAmbient"), 1, &shape.primitive.material.cAmbient[0]);
//         glUniform4fv(glGetUniformLocation(m_phong_shader, "cDiffuse"), 1, &shape.primitive.material.cDiffuse[0]);
//         glUniform4fv(glGetUniformLocation(m_phong_shader, "cSpecular"), 1, &shape.primitive.material.cSpecular[0]);

//         GLuint vaoAddress, vboAddress, vertexCount = 0;
//         switch(shape.primitive.type){
//         case PrimitiveType::PRIMITIVE_CUBE:
//             vboAddress = m_cube_vbo;
//             vaoAddress = m_cube_vao;
//             vertexCount = m_cube_vertexCount;
//             break;
//         case PrimitiveType::PRIMITIVE_CONE:
//             vboAddress = m_cone_vbo;
//             vaoAddress = m_cone_vao;
//             vertexCount = m_cone_vertexCount;
//             break;
//         case PrimitiveType::PRIMITIVE_CYLINDER:
//             vboAddress = m_cyl_vbo;
//             vaoAddress = m_cyl_vao;
//             vertexCount = m_cyl_vertexCount;
//             break;
//         case PrimitiveType::PRIMITIVE_SPHERE:
//             vboAddress = m_sphere_vbo;
//             vaoAddress = m_sphere_vao;
//             vertexCount = m_sphere_vertexCount;
//             break;
//         case PrimitiveType::PRIMITIVE_MESH:
//             break;
//         }
//         glBindBuffer(GL_ARRAY_BUFFER, vboAddress);
//         glBindVertexArray(vaoAddress);
//         glDrawArrays(GL_TRIANGLES, 0, vertexCount);
//     }
//     glBindVertexArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
// }

void Realtime::blurBrightTexture() {
    // glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    // glDisable(GL_DEPTH_TEST);
    // glDepthMask(GL_FALSE);

    bool horizontal = true, first_iteration = true;
    int amount = 20; // blur passes

    glUseProgram(m_blur_shader);
    glUniform1i(glGetUniformLocation(m_blur_shader, "image"), 0);
    for(int i = 0; i < amount; i++){
        glBindFramebuffer(GL_FRAMEBUFFER, pingpong_fbo[horizontal]);
        glUniform1i(glGetUniformLocation(m_blur_shader, "horizontal"), horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_bright_texture : pingpong_colorBuffers[!horizontal]);

        glBindVertexArray(m_fullscreen_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        if(first_iteration) first_iteration = false;
    }
    // glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    // glViewport(0, 0, m_fbo_width, m_fbo_height);

    // glDepthMask(GL_TRUE);
    // glEnable(GL_DEPTH_TEST);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    aspect = (float)(size().width() * m_devicePixelRatio)
             / (float)(size().height() * m_devicePixelRatio);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteTextures(1, &m_bright_texture);
    glDeleteTextures(1, &first_depth_fbo);
    glDeleteRenderbuffers(1, &m_rbo);
    glDeleteFramebuffers(1, &m_fbo);

    m_fbo_width = size().width() * m_devicePixelRatio;
    m_fbo_height = size().height() * m_devicePixelRatio;

    makeFBO();
}

/**
 * @brief Realtime::sceneChanged
 * @param scenePath
 */
void Realtime::sceneChanged() {
    makeCurrent();
    RenderData metaData;
    SceneParser::parse(settings.sceneFilePath, metaData);
    inputs.initialize(metaData);
    std::vector<RenderShapeData> shapes = inputs.getPrimitives();
    Bodies.resize(shapes.size());
    for (int i = 0; i < shapes.size(); i++) {
        Bodies[i] = *shapes[i].rb;
    }
    n_bodies = shapes.size();
    cam = inputs.getCamera();
    doneCurrent();
    m_pendingScene = false;
    update(); // asks for a PaintGL() call to occur
}

/**
 * @brief Realtime::settingsChanged
 * Updates the vbos and the projection matrix
 */
void Realtime::settingsChanged() {
    if (!isValid() || m_phong_shader == 0) {
        m_pendingSettingsUpdate = true;
        return;
    }
    makeCurrent();
    setupVBO(m_cone_vao, m_cone_vbo, PrimitiveType::PRIMITIVE_CONE);
    setupVBO(m_cube_vao, m_cube_vbo, PrimitiveType::PRIMITIVE_CUBE);
    setupVBO(m_cyl_vao, m_cyl_vbo, PrimitiveType::PRIMITIVE_CYLINDER);
    setupVBO(m_sphere_vao, m_sphere_vbo, PrimitiveType::PRIMITIVE_SPHERE);
    cam.calculatePerspectiveMatrix((float)settings.nearPlane, (float) settings.farPlane, cam.getHeightAngle(),
                                   aspect);

    zfar_plane = settings.farPlane;
    znear_plane = settings.nearPlane;
    aperature = settings.aperature;
    focal_length = settings.focal_length;
    plane_in_focus = settings.plane_in_focus;

    makeFBO();
    doneCurrent();
    m_pendingSettingsUpdate = false;

    update(); // asks for a PaintGL() call to occur
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
    if (!event->buttons().testFlag(Qt::LeftButton)) m_mouseDown = false;
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);
        cam.rotateCamera(deltaX, deltaY);
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    if (m_pendingScene) return;
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    double t0 = elapsedms * 0.001;
    double t1 = t0 + deltaTime;
    int len = STATE_SIZE * n_bodies;
    std::vector<double> y0(len);
    std::vector<double> yEnd(len);

  //Update input forces
    for (int i = 0; i < n_bodies; i++) {
        RigidBody& rb = Bodies[i];

        rb.inputForce = glm::vec3(0.0f);
        rb.inputTorque = glm::vec3(0.0f);

        const float move = 1.0f;    // direction only; scaling done in Compute_Force
        const float rotate = 1.0f;

        // Movement (XZ plane)
        if (m_keyMap[Qt::Key_Up]) rb.inputForce.z -= move;
        if (m_keyMap[Qt::Key_Down]) rb.inputForce.z += move;
        if (m_keyMap[Qt::Key_Left]) rb.inputForce.x -= move;
        if (m_keyMap[Qt::Key_Right]) rb.inputForce.x += move;

        // Rotation (yaw torque)
        if (m_keyMap[Qt::Key_Q]) rb.inputTorque.y += rotate;
        if (m_keyMap[Qt::Key_E]) rb.inputTorque.y -= rotate;
        if (m_keyMap[Qt::Key_Z]) rb.inputTorque.x -= rotate;
        if (m_keyMap[Qt::Key_C]) rb.inputTorque.z += rotate;
    }

    Phys::Bodies_to_Array(y0, n_bodies, &Bodies[0]);
    Phys::ode(y0, yEnd, len, t0, t1, n_bodies, &Bodies[0], deltaTime);
    Phys::Array_to_Bodies(yEnd, &Bodies[0], n_bodies);
    for (int i = 0; i < n_bodies; i++)
    {
        RigidBody& rb = Bodies[i];

        rb.v  = rb.P * rb.invMass;
        rb.R  = glm::mat3_cast(rb.q);
        rb.I_inv = rb.R * rb.I_body_inv * glm::transpose(rb.R);
        rb.w  = rb.I_inv * rb.L;

        // update transform matrix
        glm::mat4 S = glm::scale(glm::mat4(1.0f), rb.scale);   // scale
        glm::mat4 R = glm::mat4_cast(rb.q);                     // rotation
        glm::mat4 T = glm::translate(glm::mat4(1.0f), rb.x);
        inputs.getPrimitives()[i].ctm = T * R* S;
    }

    m_elapsedTimer.restart();
    if (m_keyMap[Qt::Key_W]) cam.translateCamera(Qt::Key_W, deltaTime);
    if (m_keyMap[Qt::Key_S]) cam.translateCamera(Qt::Key_S, deltaTime);
    if (m_keyMap[Qt::Key_A]) cam.translateCamera(Qt::Key_A, deltaTime);
    if (m_keyMap[Qt::Key_D]) cam.translateCamera(Qt::Key_D, deltaTime);
    if (m_keyMap[Qt::Key_Space]) cam.translateCamera(Qt::Key_Space, deltaTime);
    if (m_keyMap[Qt::Key_Control]) cam.translateCamera(Qt::Key_Control, deltaTime);
    camera_pos = glm::vec4(cam.pos, 1.0);
    update(); // triggers paintGL()
}

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

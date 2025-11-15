#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils/Triangle.h"
#include "utils/sceneparser.h"


// ================== Helper functions!!!
glm::mat4 Realtime::makePerspectiveMatrix(float fovY, float aspect, float near, float far) {
    float tanHalfFov = tan(fovY / 2.0f); //fovY in radians
    glm::mat4 proj(0.0f);
    proj[0][0] = 1.0f / (aspect * tanHalfFov);
    proj[1][1] = 1.0f / tanHalfFov;
    proj[2][2] = - (far + near) / (far - near);
    proj[2][3] = -1.0f;
    proj[3][2] = - (2.0f * far * near) / (far - near);
    proj[3][3] = 0.0f;

    return proj;
}

//creating VBO/VAO for primitives
void Realtime::setupShapeVAO(PrimitiveType type) {
    GLuint vbo_id;
    GLuint vao_id;
    glGenBuffers(1, &vbo_id);
    glGenVertexArrays(1, &vao_id);

    m_vbo_map[type] = vbo_id;
    m_vao_map[type] = vao_id;

    glBindVertexArray(vao_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);

    GLsizei stride = 6 * sizeof(GLfloat);
    glEnableVertexAttribArray(0); // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1); // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::updateAllShapeTessellations() {
    int param1 = settings.shapeParameter1;
    int param2 = settings.shapeParameter2;

    // ------ Cube ------
    m_cube.updateParams(param1);
    std::vector<float> cubeData = m_cube.generateShape();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_map[PrimitiveType::PRIMITIVE_CUBE]);
    glBufferData(GL_ARRAY_BUFFER, cubeData.size() * sizeof(GLfloat), cubeData.data(), GL_STATIC_DRAW);
    m_vertex_count_map[PrimitiveType::PRIMITIVE_CUBE] = cubeData.size() / 6;

    // ------ Sphere ------
    m_sphere.updateParams(param1, param2);
    std::vector<float> sphereData = m_sphere.generateShape();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_map[PrimitiveType::PRIMITIVE_SPHERE]);
    glBufferData(GL_ARRAY_BUFFER, sphereData.size() * sizeof(GLfloat), sphereData.data(), GL_STATIC_DRAW);
    m_vertex_count_map[PrimitiveType::PRIMITIVE_SPHERE] = sphereData.size() / 6;

    // ------ Cylinder ------
    m_cylinder.updateParams(param1, param2);
    std::vector<float> cylinderData = m_cylinder.generateShape();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_map[PrimitiveType::PRIMITIVE_CYLINDER]);
    glBufferData(GL_ARRAY_BUFFER, cylinderData.size() * sizeof(GLfloat), cylinderData.data(), GL_STATIC_DRAW);
    m_vertex_count_map[PrimitiveType::PRIMITIVE_CYLINDER] = cylinderData.size() / 6;

    // ------ Cone ------
    m_cone.updateParams(param1, param2);
    std::vector<float> coneData = m_cone.generateShape();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_map[PrimitiveType::PRIMITIVE_CONE]);
    glBufferData(GL_ARRAY_BUFFER, coneData.size() * sizeof(GLfloat), coneData.data(), GL_STATIC_DRAW);
    m_vertex_count_map[PrimitiveType::PRIMITIVE_CONE] = coneData.size() / 6;

    // 共通のバインド解除
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    //delete the VBO, VAO, Shader
    for (auto const& [type, id] : m_vbo_map) {
        glDeleteBuffers(1, &id);
    }
    for (auto const& [type, id] : m_vao_map) {
        glDeleteVertexArrays(1, &id);
    }
    glDeleteProgram(m_shader_program);
    this->doneCurrent();
}

void Realtime::initializeGL() {
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
    // Teflls OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    m_shader_program = ShaderLoader::createShaderProgram("resources/shaders/default.vert",
                                                         "resources/shaders/default.frag");

    //setting up VAO and VBO for all primitives
    setupShapeVAO(PrimitiveType::PRIMITIVE_CUBE);
    setupShapeVAO(PrimitiveType::PRIMITIVE_SPHERE);
    setupShapeVAO(PrimitiveType::PRIMITIVE_CYLINDER);
    setupShapeVAO(PrimitiveType::PRIMITIVE_CONE);

    updateAllShapeTessellations();

    // //todo:dummy primitive lists to draw
    // m_shapes_to_draw.clear();
    // RenderShapeData dummy_shape;
    // dummy_shape.primitive.type = PrimitiveType::PRIMITIVE_CYLINDER;
    // dummy_shape.ctm = glm::mat4(1.0f);
    // m_shapes_to_draw.push_back(dummy_shape);

    //for avoiding crashes
    m_gl_initialized = true;
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader_program);

    //A:Projection matrix setting
    float aspectRatio = (float)width() / (float)height();
    float fovY = m_renderData.cameraData.heightAngle; //potential bug: is this radians?
    float nearPlane = settings.nearPlane;
    float farPlane = settings.farPlane;
    glm::mat4 proj = makePerspectiveMatrix(fovY, aspectRatio, nearPlane, farPlane);

    //B:View Matrix setting
    glm::mat4 view = glm::lookAt(
        glm::vec3(m_renderData.cameraData.pos),
        glm::vec3(m_renderData.cameraData.look),
        glm::vec3(m_renderData.cameraData.up)
        );

    //C:Send the shared matrixes to shaders
    //c.1:lighting global setting
    GLint kaLoc = glGetUniformLocation(m_shader_program, "u_global.ka");
    GLint kdLoc = glGetUniformLocation(m_shader_program, "u_global.kd");
    GLint ksLoc = glGetUniformLocation(m_shader_program, "u_global.ks");
    glUniform1f(kaLoc, m_renderData.globalData.ka);
    glUniform1f(kdLoc, m_renderData.globalData.kd);
    glUniform1f(ksLoc, m_renderData.globalData.ks);

    //c.2 Camera pos
    GLint camPosLoc = glGetUniformLocation(m_shader_program, "u_camPos");
    glUniform3fv(camPosLoc, 1, &m_renderData.cameraData.pos[0]);

    //c.3 light amounts
    int numLights = std::min((int)m_renderData.lights.size(), 10);
    GLint numLightsLoc = glGetUniformLocation(m_shader_program, "u_numLights");
    glUniform1i(numLightsLoc, numLights);

    //c.4 light array
    for (int i = 0; i < numLights; ++i) {
        std::string prefix = "u_lights[" + std::to_string(i) + "].";
        glUniform1i(glGetUniformLocation(m_shader_program, (prefix + "type").c_str()), (int)m_renderData.lights[i].type);
        glUniform4fv(glGetUniformLocation(m_shader_program, (prefix + "color").c_str()), 1, &m_renderData.lights[i].color[0]);
        glUniform3fv(glGetUniformLocation(m_shader_program, (prefix + "function").c_str()), 1, &m_renderData.lights[i].function[0]);
        glUniform4fv(glGetUniformLocation(m_shader_program, (prefix + "pos").c_str()), 1, &m_renderData.lights[i].pos[0]);
        glUniform4fv(glGetUniformLocation(m_shader_program, (prefix + "dir").c_str()), 1, &m_renderData.lights[i].dir[0]);
    }

    GLint modelLoc = glGetUniformLocation(m_shader_program, "u_model");

    //c.5 view, proj
    GLint viewLoc = glGetUniformLocation(m_shader_program, "u_view");
    GLint projLoc = glGetUniformLocation(m_shader_program, "u_proj");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);


    //D:Model matrix, for loop for each primitives
    for (const RenderShapeData &shape : m_renderData.shapes) {
        if (m_vao_map.count(shape.primitive.type)) {//check if there is VAO for primitive type
            //d.1 model matrix
            glBindVertexArray(m_vao_map.at(shape.primitive.type));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &shape.ctm[0][0]);

            //d.2 material data
            GLint cAmbLoc = glGetUniformLocation(m_shader_program, "u_material.cAmbient");
            GLint cDiffLoc = glGetUniformLocation(m_shader_program, "u_material.cDiffuse");
            GLint cSpecLoc = glGetUniformLocation(m_shader_program, "u_material.cSpecular");
            GLint shinyLoc = glGetUniformLocation(m_shader_program, "u_material.shininess");

            glUniform4fv(cAmbLoc, 1, &shape.primitive.material.cAmbient[0]);
            glUniform4fv(cDiffLoc, 1, &shape.primitive.material.cDiffuse[0]);
            glUniform4fv(cSpecLoc, 1, &shape.primitive.material.cSpecular[0]);
            glUniform1f(shinyLoc, shape.primitive.material.shininess);

            //d.3 draw
            GLuint vertexCount = m_vertex_count_map.at(shape.primitive.type);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        }
    }

    //E:Cleanup after drawing
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::sceneChanged() {
    std::string filepath = settings.sceneFilePath;
    m_renderData.lights.clear();
    //error
    if (!m_parser.parse(filepath, m_renderData)) {
        std::cerr << "Error parsing scene: " << filepath << std::endl;
        return;
    }
    updateAllShapeTessellations();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (m_gl_initialized) {
        updateAllShapeTessellations();
    }
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
        //1.where is camera
        glm::vec3 look = glm::vec3(m_renderData.cameraData.look);
        glm::vec3 up   = glm::vec3(m_renderData.cameraData.up);

        //2.sens
        float sensitivity = 0.1f;

        //3.rotation angle
        float yawAngle = glm::radians(-sensitivity * deltaX);
        float pitchAngle = glm::radians(-sensitivity * deltaY);

        // 4. Yaw
        glm::mat4 yawRot = glm::rotate(glm::mat4(1.0f), yawAngle, glm::vec3(0, 1, 0));
        look = glm::vec3(yawRot * glm::vec4(look, 0.0f));
        up   = glm::vec3(yawRot * glm::vec4(up, 0.0f));

        // 5. Pitch
        glm::vec3 right = glm::normalize(glm::cross(look, up));
        glm::mat4 pitchRot = glm::rotate(glm::mat4(1.0f), pitchAngle, right);
        look = glm::vec3(pitchRot * glm::vec4(look, 0.0f));
        up   = glm::vec3(pitchRot * glm::vec4(up, 0.0f));

        //6. normalize and update
        look = glm::normalize(look);
        up   = glm::normalize(up);
        m_renderData.cameraData.look = glm::vec4(look, 0.0f);
        m_renderData.cameraData.up   = glm::vec4(up, 0.0f);


        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    //1. camera rn
    glm::vec3 pos  = glm::vec3(m_renderData.cameraData.pos);
    glm::vec3 look = glm::vec3(m_renderData.cameraData.look);
    glm::vec3 up   = glm::vec3(m_renderData.cameraData.up);

    //2. right
    glm::vec3 right = glm::normalize(glm::cross(look, up));

    //3. speed
    float speed = 5.0f;

    // 4. new pos by key
    if (m_keyMap[Qt::Key_W]) {
        pos += look * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_S]) {
        pos -= look * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_D]) {
        pos += right * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_A]) {
        pos -= right * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_Space]) {
        pos += glm::vec3(0, 1, 0) * speed * deltaTime;
    }
    if (m_keyMap[Qt::Key_Control]) {
        pos -= glm::vec3(0, 1, 0) * speed * deltaTime;
    }

    //new camera data
    m_renderData.cameraData.pos = glm::vec4(pos, 1.0f);

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

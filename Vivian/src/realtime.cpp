#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include <glm/gtx/string_cast.hpp>



#include "settings.h"
#include "utils/phys.h"
#include "utils/sceneparser.h"
#include "shaderloader.h"
#include "utils/shapes/Cone.h"
#include "utils/shapes/Cube.h"
#include "utils/shapes/Cylinder.h"
#include "utils/shapes/Sphere.h"
#include "camera.h"
#include "camera.cpp"
#include "utils/scenedata.h"



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

}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    glDeleteProgram(m_shader);

    glDeleteProgram(m_fullscreen_shader);


    glDeleteBuffers(1, &m_cube_vbo);
    m_cube_vbo = 0;

    glDeleteVertexArrays(1, &m_cube_vao);
    m_cube_vao = 0;

    glDeleteBuffers(1, &m_cylinder_vbo);
    m_cylinder_vbo = 0;

    glDeleteVertexArrays(1, &m_cylinder_vao);
    m_cylinder_vao = 0;

    glDeleteBuffers(1, &m_sphere_vbo);
    m_sphere_vbo = 0;

    glDeleteVertexArrays(1, &m_sphere_vao);
    m_sphere_vao = 0;

    glDeleteBuffers(1, &m_cone_vbo);
    m_cone_vbo = 0;

    glDeleteVertexArrays(1, &m_cone_vao);
    m_cone_vao = 0;

    glDeleteTextures(1, &m_texture);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);


    this->doneCurrent();


}



//called before anything happens
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
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);


    // Students: anything requiring OpenGL calls when the program starts should be done here

    // // Task 4: Set the clear color here
    glClearColor(0, 0, 0, 1.0);


    // Shader setup (DO NOT EDIT)
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_fullscreen_shader = ShaderLoader::createShaderProgram("/Users/vhcch/Desktop/Vivian/brown/cs1230/project-6-final-project-gear-up-vhchen29/resources/shaders/texture.vert",
                                                            "/Users/vhcch/Desktop/Vivian/brown/cs1230/project-6-final-project-gear-up-vhchen29/resources/shaders/texture.frag");
    // m_background_shader = ShaderLoader::createShaderProgram("/Users/vhcch/Desktop/Vivian/brown/cs1230/project-6-final-project-gear-up-vhchen29/resources/shaders/background.vert",
    //                                                         "/Users/vhcch/Desktop/Vivian/brown/cs1230/project-6-final-project-gear-up-vhchen29/resources/shaders/background.frag");


    // m_background_image = QImage(":/scenefiles/bump-mapping/backgrounds/IMG_4368.jpg")
    //                         .convertToFormat(QImage::Format_RGBA8888).mirrored();

    // if (m_background_image.isNull()) {
    //     std::cout << "Background FAILED to load!" << std::endl;
    // }

    // glGenTextures(1, &m_background);
    // glBindTexture(GL_TEXTURE_2D, m_background);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
    //              m_background_image.width(), m_background_image.height(),
    //              0, GL_RGBA, GL_UNSIGNED_BYTE, m_background_image.bits());

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glBindTexture(GL_TEXTURE_2D, 0);





    bindTexture(); //bump texture

    //initialize FBOS

    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;


    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
            -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
            1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,   1.0f, 1.0f
        };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    makeFBO();


    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}


void Realtime::bindVBOVAO(PrimitiveType primType, std::vector<GLfloat> vertices){

    if(primType == PrimitiveType::PRIMITIVE_CUBE){

        // ================== Vertex Buffer Objects

        glGenBuffers(1, &m_cube_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        m_cube_vertices = vertices.size();

        // ================== Vertex Array Objects

        glGenVertexArrays(1, &m_cube_vao);
        glBindVertexArray(m_cube_vao);

        //positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(0));

        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        //uvs
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

        //Pu
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(8 * sizeof(GLfloat)));

        //Pv
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(11 * sizeof(GLfloat)));



        // ================== Returning to Default State

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    } else if(primType == PrimitiveType::PRIMITIVE_CYLINDER){

        // ================== Vertex Buffer Objects

        glGenBuffers(1, &m_cylinder_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        m_cylinder_vertices = vertices.size();

        // ================== Vertex Array Objects

        glGenVertexArrays(1, &m_cylinder_vao);
        glBindVertexArray(m_cylinder_vao);

        //positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(0));

        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        //uvs
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

        //Pu
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(8 * sizeof(GLfloat)));

        //Pv
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(11 * sizeof(GLfloat)));


        // ================== Returning to Default State

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    } else if(primType == PrimitiveType::PRIMITIVE_SPHERE){

        // ================== Vertex Buffer Objects

        glGenBuffers(1, &m_sphere_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        m_sphere_vertices = vertices.size();

        // ================== Vertex Array Objects

        glGenVertexArrays(1, &m_sphere_vao);
        glBindVertexArray(m_sphere_vao);

        //positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(0));

        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        //uvs
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

        //Pu
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(8 * sizeof(GLfloat)));

        //Pv
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(11 * sizeof(GLfloat)));


        // ================== Returning to Default State

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    } else if(primType == PrimitiveType::PRIMITIVE_CONE){
        // ================== Vertex Buffer Objects

        glGenBuffers(1, &m_cone_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        m_cone_vertices = vertices.size();

        // ================== Vertex Array Objects

        glGenVertexArrays(1, &m_cone_vao);
        glBindVertexArray(m_cone_vao);

        //positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(0));

        //normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

        //uvs
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(6 * sizeof(GLfloat)));

        //Pu
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(8 * sizeof(GLfloat)));

        //Pv
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), reinterpret_cast<void*>(11 * sizeof(GLfloat)));


        // ================== Returning to Default State

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

    }

}

//bind bump texture to the shader

void Realtime::bindTexture(){
    if (settings.hasTexture){
        // QString filepath = QString("/Users/vhcch/Desktop/Vivian/brown/cs1230/project-6-final-project-gear-up-vhchen29/scenefiles/bump-mapping/topleft.png");

        m_texture_image = settings.bumpTexture;

        // Task 2: Format image to fit OpenGL
        // m_texture_image = m_texture_image.convertToFormat(QImage::Format_Grayscale8).mirrored();

        // Task 3: Generate texture
        glGenTextures(1, &m_texture);

        // Task 9: Set the active texture slot to texture slot 0
        glActiveTexture(GL_TEXTURE0);

        // Task 4: Bind texture
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // Task 5: Load image into  texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_texture_image.width(), m_texture_image.height(), 0, GL_RED, GL_UNSIGNED_BYTE, m_texture_image.bits());

        // Task 6: Set min and mag filters' interpolation mode to linear
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Task 7: Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(m_shader);
        GLint samplerLoc = glGetUniformLocation(m_shader, "u_texture");
        std::cout << "u_texture location = " << samplerLoc << std::endl;
        glUniform1i(samplerLoc, 0);
        glUseProgram(0);
    }



    glUseProgram(m_shader);
    glUniform1i(glGetUniformLocation(m_shader, "hasTexture"), settings.hasTexture);
    glUniform1i(glGetUniformLocation(m_shader, "bump_depth"), settings.bumpDepth);
    glUseProgram(0);

}

void Realtime::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Task 20: Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Task 21: Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}



//helper for applying uniforms: ctm transforms, mvp, lighting
void Realtime::makeShape(GLuint vbo, GLuint vao, int vertexCount, const glm::mat4 &model, const SceneMaterial &material){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);


    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    glm::mat4 mvp = m_proj * m_view * model;

    glUniformMatrix3fv(glGetUniformLocation(m_shader, "normalMatrix"), 1, GL_FALSE, &normalMatrix[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &mvp[0][0]);


    glUniform1f(glGetUniformLocation(m_shader, "k_a"), metaData.globalData.ka);
    glUniform1f(glGetUniformLocation(m_shader, "k_d"), metaData.globalData.kd);
    glUniform1f(glGetUniformLocation(m_shader, "k_s"), metaData.globalData.ks);

    glUniform4fv(glGetUniformLocation(m_shader, "world_camera"), 1, &metaData.cameraData.pos[0]);


    glUniform1i(glGetUniformLocation(m_shader, "num_lights"), metaData.lights.size());

    for(int i = 0; i < metaData.lights.size(); i++){

        SceneLightData light = metaData.lights[i];
        std::string base = "lights[" + std::to_string(i) + "]";


        //type
        glUniform1i(glGetUniformLocation(m_shader, (base + ".type").c_str()), static_cast<int>(light.type));

        //color
        glUniform3fv(glGetUniformLocation(m_shader, (base + ".color").c_str()), 1, &light.color[0]);
        // std::cout << "light color: " << glm::to_string(light.color) << std::endl;

        //fatt
        glUniform3fv(glGetUniformLocation(m_shader, (base + ".function").c_str()), 1, &light.function[0]);

        //pos
        glUniform4fv(glGetUniformLocation(m_shader, (base + ".pos").c_str()), 1, &light.pos[0]);

        //dir
        glUniform4fv(glGetUniformLocation(m_shader, (base + ".dir").c_str()), 1, &light.dir[0]);

        //penumbra
        glUniform1f(glGetUniformLocation(m_shader, (base + ".penumbra").c_str()), light.penumbra);

        //angle
        glUniform1f(glGetUniformLocation(m_shader, (base + ".angle").c_str()), light.angle);

    }

    //cAmbient
    glUniform3fv(glGetUniformLocation(m_shader, "material.cAmbient"), 1, &material.cAmbient[0]);

    //cDiffuse
    glUniform3fv(glGetUniformLocation(m_shader, "material.cDiffuse"), 1, &material.cDiffuse[0]);

    //cSpecular
    glUniform3fv(glGetUniformLocation(m_shader, "material.cSpecular"), 1, &material.cSpecular[0]);

    //shininess
    glUniform1f(glGetUniformLocation(m_shader, "material.shininess"), material.shininess);



    glDrawArrays(GL_TRIANGLES, 0, vertexCount / 14);
    glBindVertexArray(0);
}


//getting called at like every miisecond possible
void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here

    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;

    // Task 24: Bind our FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_screen_width, m_screen_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(m_shader);


    glUniform1i(glGetUniformLocation(m_shader, "bump_depth"), settings.bumpDepth);

    if (settings.hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    } else {
        glDisable(GL_TEXTURE_2D);
    }


    for (const auto &s : metaData.shapes) {

        if(s.primitive.type == PrimitiveType::PRIMITIVE_CUBE){
            makeShape(m_cube_vbo, m_cube_vao, m_cube_vertices, s.ctm, s.primitive.material);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER){
            makeShape(m_cylinder_vbo, m_cylinder_vao, m_cylinder_vertices, s.ctm, s.primitive.material);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_SPHERE){
            makeShape(m_sphere_vbo, m_sphere_vao, m_sphere_vertices, s.ctm, s.primitive.material);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_CONE){
            makeShape(m_cone_vbo, m_cone_vao, m_cone_vertices, s.ctm, s.primitive.material);
        }
    }

    // Unbind the shader
    glUseProgram(0);

    // bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glBindTexture(GL_TEXTURE_2D, 0);
    paintTexture(m_fbo_texture, settings.invert || settings.color);

}

void Realtime::paintBackground(GLuint bg){

    glUseProgram(m_background_shader);

    glBindVertexArray(m_fullscreen_vao);

    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bg);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);


}

void Realtime::paintTexture(GLuint texture, bool filter){
    glUseProgram(m_fullscreen_shader);
    // Task 32: Set your bool uniform on whether or not to filter the texture drawn
    GLint loc = glGetUniformLocation(m_fullscreen_shader, "bool_texture");
    glUniform1i(loc, filter);
    // std::cout << filter << " ";

    glUniform1i(glGetUniformLocation(m_fullscreen_shader, "filter_type"), m_filter_type);

    glBindVertexArray(m_fullscreen_vao);

    // Task 10: Bind "texture" to slot 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::sceneChanged() {

    makeCurrent(); // Ensure OpenGL context is active

    metaData.shapes.clear();
    metaData.lights.clear();

    //parse scene on file loading
    SceneParser::parse(settings.sceneFilePath, metaData);

    //make camera
    glm::vec4 pos = metaData.cameraData.pos;
    glm::vec4 look = metaData.cameraData.look;
    glm::vec4 up = metaData.cameraData.up;
    float heightAngle = metaData.cameraData.heightAngle;
    float near = settings.nearPlane;
    float far  = settings.farPlane;

    m_camera = Camera(width(), height(), pos, look, up, heightAngle, near, far);
    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjMatrix();


    for( auto &s : metaData.shapes){
        s.rb = new RigidBody();
        RigidBody &rb = *s.rb;

        rb.mass = 1.0f;
        rb.invMass = 1.0f / rb.mass;

        glm::vec3 scale(
            glm::length(glm::vec3(s.ctm[0])),
            glm::length(glm::vec3(s.ctm[1])),
            glm::length(glm::vec3(s.ctm[2]))
            );
        rb.scale = scale;
        if(s.primitive.type == PrimitiveType::PRIMITIVE_CUBE){
            Cube cube;
            cube.updateParams(settings.shapeParameter1);
            rb.I_body = cube.inertiaTensor(rb.mass, scale);
            std::vector<GLfloat> vertices = cube.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_CUBE, vertices);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER){
            Cylinder cylinder;
            cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            rb.I_body = cylinder.inertiaTensor(rb.mass, scale);
            std::vector<GLfloat> vertices = cylinder.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_CYLINDER, vertices);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_SPHERE){
            Sphere sphere;
            sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            rb.I_body = sphere.inertiaTensor(rb.mass, scale);
            std::vector<GLfloat> vertices = sphere.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_SPHERE, vertices);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_CONE){
            Cone cone;
            cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            rb.I_body = cone.inertiaTensor(rb.mass, scale);
            std::vector<GLfloat> vertices = cone.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_CONE, vertices);

        }
        rb.I_body_inv = glm::inverse(rb.I_body);

        // translation from CTM

        rb.I_body_inv = glm::inverse(rb.I_body);

        // Position
        rb.x = glm::vec3(s.ctm[3]);

        // Orientation
        rb.q = glm::quat_cast(s.ctm);

        // Zero velocities
        rb.P = glm::vec3(0);
        rb.L = glm::vec3(0);
        rb.v = glm::vec3(0);
        rb.w = glm::vec3(0);

        // Force/torque accumulators
        rb.force = glm::vec3(0);
        rb.torque = glm::vec3(0);
    }

    Bodies.resize(metaData.shapes.size());
    for (int i = 0; i < metaData.shapes.size(); i++) {
        Bodies[i] = *metaData.shapes[i].rb;
    }
    n_bodies = metaData.shapes.size();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    makeCurrent(); // Ensure OpenGL context is active

    m_camera.updateProjMatrix(settings.nearPlane, settings.farPlane);
    m_view = m_camera.getViewMatrix();
    m_proj = m_camera.getProjMatrix();

    if(settings.invert){
        m_filter_type = 0;
    } else if(settings.color){
        m_filter_type = 1;
    }

    for( auto &s : metaData.shapes){

        if(s.primitive.type == PrimitiveType::PRIMITIVE_CUBE){
            Cube cube;
            cube.updateParams(settings.shapeParameter1);
            std::vector<GLfloat> vertices = cube.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_CUBE, vertices);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_CYLINDER){
            Cylinder cylinder;
            cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            std::vector<GLfloat> vertices = cylinder.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_CYLINDER, vertices);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_SPHERE){
            Sphere sphere;
            sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            std::vector<GLfloat> vertices = sphere.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_SPHERE, vertices);

        } else if(s.primitive.type == PrimitiveType::PRIMITIVE_CONE){
            Cone cone;
            cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            std::vector<GLfloat> vertices = cone.generateShape();
            bindVBOVAO(PrimitiveType::PRIMITIVE_CONE, vertices);

        }
    }



    //load texture into textureReader
    //get vector of in the format of (bump value , u coord, v coord,)
    //calculate a, b, c for each (a vector 3 times the l


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

        float sensitivity = 0.007f;

        float xAngle   = deltaX * sensitivity;
        float yAngle = deltaY * sensitivity;   // around camera right

        //rodrigues rotation function
        m_camera.rotateX(xAngle);
        m_camera.rotateY(yAngle);


        m_view = m_camera.getViewMatrix();

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
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
        metaData.shapes[i].ctm = T * R* S;
    }

    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    glm::vec3 movement(0.f);

    float speed = 5.f;

    glm::vec3 forward = glm::normalize(glm::vec3(m_camera.m_look));
    glm::vec3 up = glm::normalize(glm::vec3(m_camera.m_up));
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    if (m_keyMap[Qt::Key_W]) {
        movement += forward;
    }
    if (m_keyMap[Qt::Key_S]) {
        movement -= forward;
    }
    if (m_keyMap[Qt::Key_A]) {
        movement -= right;
    }
    if (m_keyMap[Qt::Key_D]) {
        movement += right;
    }
    if (m_keyMap[Qt::Key_Space]) {
        movement += glm::vec3(0.f, 1.f, 0.f);  // world up
    }
    if (m_keyMap[Qt::Key_Control]) {
        movement -= glm::vec3(0.f, 1.f, 0.f);  // world down
    }

    if (glm::length(movement) > 0.001f) {
        movement = glm::normalize(movement) * speed * deltaTime;
        m_camera.translate(movement);
        m_view = m_camera.getViewMatrix();
    }

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

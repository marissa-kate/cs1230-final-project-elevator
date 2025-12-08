#include "particlesystem.h"
#include <cmath>
#include <vector>
#include <iostream>


ParticleSystem::ParticleSystem(const SceneParticleEmitter& config)
    : m_config(config), m_vao(0), m_vbo_mesh(0), m_vbo_instance(0)
{
}

ParticleSystem::~ParticleSystem() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo_mesh) glDeleteBuffers(1, &m_vbo_mesh);
    if (m_vbo_instance) glDeleteBuffers(1, &m_vbo_instance);
}

void ParticleSystem::init() {
    int safeMaxParticles = std::min(100000000, std::max(0, m_config.maxParticles));
    m_particles.resize(safeMaxParticles);         // robust to negative
    if (m_particles.empty()) {
        return;
    }
    if (m_config.lsysIter > 0) { //for lsystem driven design
        LSystemConfig lsysConfig;
        lsysConfig.axiom = m_config.lsysAxiom;
        lsysConfig.rule = m_config.lsysRule;
        lsysConfig.iterations = std::min(6, m_config.lsysIter); //for safety
        lsysConfig.angle = m_config.lsysAngle;
        lsysConfig.length = 1.0f; // base length
        lsysConfig.startPos = m_config.position; // starting position

        LSystem lsys(lsysConfig);
        skeleton = lsys.generate(); // generate skeleton
    }

    float safeLifetime = (std::abs(m_config.lifetime) < 0.0001f) ? 0.0001f : m_config.lifetime;

    for (auto& p : m_particles) {
        float age = frand() * safeLifetime;
        p.life = 1.0f - (age / safeLifetime);

        float r = (frand() * 1.5f) + 0.5f;
        float startTheta = frand() * 6.28f;


        if (!skeleton.empty()) {//L-system mode
            int segIndex = rand() % skeleton.size();
            Segment& seg = skeleton[segIndex];
            float t = frand();

            p.position = glm::mix(seg.start, seg.end, t);
            // distribute
            p.position += glm::vec3(frand()-0.5f, frand()-0.5f, frand()-0.5f) * 0.1f;

            // velocity is slow, because it's a tree
            p.velocity = m_config.velocity;
        } else {
            // spinning up for normal design
            float currentY = m_config.position.y + (p.velocity.y * age);
            float speedVariation = 0.9f + (frand() * 0.2f);
            p.velocity = m_config.velocity * speedVariation;
            float rotationSpeed = 2.0f * speedVariation;
            float currentTheta = startTheta + (rotationSpeed * age);
            float spiralFactor = pow(0.995f, age * 60.0f);

            // m_config.position.x, z is center
            p.position.x = m_config.position.x + (r * cos(currentTheta) * spiralFactor);
            p.position.y = currentY;
            p.position.z = m_config.position.z + (r * sin(currentTheta) * spiralFactor);
        }

        p.color = m_config.color; // use set color
        p.phase = frand() * 6.28f;
    }

    // VAO/VBO Setup
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo_mesh);
    glGenBuffers(1, &m_vbo_instance);

    glBindVertexArray(m_vao);

    // Quad Mesh
    std::vector<float> quadVertices = {
        -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.5f,  0.5f, 0.0f, -0.5f, 0.5f, 0.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_mesh);
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Instance Data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instance);
    glBufferData(GL_ARRAY_BUFFER, m_particles.size() * 3 * sizeof(float), nullptr, GL_STREAM_DRAW); // 空で確保

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(2, 1); //for speed, using same data for all!

    glBindVertexArray(0);
}

void ParticleSystem::update(float deltaTime, float audioLevel, float audioFreq) {
    if (m_particles.empty()) return;


    std::vector<float> positionData;
    positionData.reserve(m_particles.size() * 3);

    // is it audio reactive?
    if (!m_config.isAudioReactive) {
        audioLevel = 0.0f;
        audioFreq = 0.0f; // set it to 0 if no
    }
    bool isLSystem = (m_config.lsysIter > 0 && !skeleton.empty());

    float safeLifetime = (m_config.lifetime <= 0.0001f) ? 0.1f : m_config.lifetime; //no minus!

    for (auto& p : m_particles) {
        p.life -= deltaTime / safeLifetime;         // avoid 0 or negative division

        if (p.life <= 0.0f) {
            p.life = 1.0f + (frand() * 0.2f);

            if (isLSystem) {
                // L-System respawn
                int segIndex = rand() % skeleton.size();
                auto& seg = skeleton[segIndex];

                float t = frand();
                p.position = glm::mix(seg.start, seg.end, t);
                p.position += glm::vec3(frand()-0.5f, frand()-0.5f, frand()-0.5f) * 0.1f; //a bit distibuted

                p.velocity = m_config.velocity; // back to speed (default is 0?)
            }else{

                float r = (frand() * 1.5f) + 0.5f;
                float theta = frand() * 6.28f;
                float startY = m_config.position.y - (frand() * 0.5f);

                p.position = glm::vec3(m_config.position.x + r * cos(theta),
                                       startY,
                                       m_config.position.z + r * sin(theta));

                float speedVariation = 0.9f + (frand() * 0.2f);
                p.velocity = m_config.velocity * speedVariation;
            }
        }

        if (isLSystem) {//L-system movement: dont roll!
        }else{
            // movement
            glm::vec3 currentVel = p.velocity;
            currentVel.y += audioLevel * 3.0f; // sound speeds up
            p.position += currentVel * deltaTime;

            // rotation
            float rotationSpeed = 4.0f * currentVel.y * deltaTime;

            // relative position for spinning
            float relX = p.position.x - m_config.position.x;
            float relZ = p.position.z - m_config.position.z;

            float nextX = relX * cos(rotationSpeed) - relZ * sin(rotationSpeed);
            float nextZ = relX * sin(rotationSpeed) + relZ * cos(rotationSpeed);

            // pulled to center
            float dynamicSuction = 0.96f + (audioLevel * 0.25f);
            p.position.x = m_config.position.x + (nextX * dynamicSuction);
            p.position.z = m_config.position.z + (nextZ * dynamicSuction);
        }

        positionData.push_back(p.position.x);
        positionData.push_back(p.position.y);
        positionData.push_back(p.position.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positionData.size() * sizeof(float), positionData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj) {

    if (m_particles.empty()) return;
    glUseProgram(shaderProgram);
    glBindVertexArray(m_vao);

    // Uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "u_proj"), 1, GL_FALSE, &proj[0][0]);

    // calculate color
    glUniform4fv(glGetUniformLocation(shaderProgram, "u_baseColor"), 1, &m_config.color[0]);

    // sending scale
    glUniform1f(glGetUniformLocation(shaderProgram, "u_scale"), m_config.scale);

    // draw
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_particles.size());

    glBindVertexArray(0);
    glUseProgram(0);
}

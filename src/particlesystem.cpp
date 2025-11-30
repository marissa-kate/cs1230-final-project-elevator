#include "particlesystem.h"
#include <cmath>
#include <vector>

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
    m_particles.resize(m_config.maxParticles);

    for (auto& p : m_particles) {
        // Pre-warming
        float age = frand() * m_config.lifetime;
        p.life = 1.0f - (age / m_config.lifetime);

        float speedVariation = 0.9f + (frand() * 0.2f);
        p.velocity = m_config.velocity * speedVariation;

        float r = (frand() * 1.5f) + 0.5f;
        float startTheta = frand() * 6.28f;

        // spinning up
        float currentY = m_config.position.y + (p.velocity.y * age);

        float rotationSpeed = 2.0f * speedVariation;
        float currentTheta = startTheta + (rotationSpeed * age);
        float spiralFactor = pow(0.995f, age * 60.0f);

        // m_config.position.x, z is center
        p.position.x = m_config.position.x + (r * cos(currentTheta) * spiralFactor);
        p.position.y = currentY;
        p.position.z = m_config.position.z + (r * sin(currentTheta) * spiralFactor);

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
    std::vector<float> positionData;
    positionData.reserve(m_particles.size() * 3);

    // is it audio reactive?
    if (!m_config.isAudioReactive) {
        audioLevel = 0.0f;
        audioFreq = 0.0f; // set it to 0 if no
    }

    for (auto& p : m_particles) {
        p.life -= deltaTime / m_config.lifetime; // life setting

        if (p.life <= 0.0f) {
            p.life = 1.0f + (frand() * 0.2f);

            float r = (frand() * 1.5f) + 0.5f;
            float theta = frand() * 6.28f;
            float startY = m_config.position.y - (frand() * 0.5f);

            p.position = glm::vec3(m_config.position.x + r * cos(theta),
                                   startY,
                                   m_config.position.z + r * sin(theta));

            float speedVariation = 0.9f + (frand() * 0.2f);
            p.velocity = m_config.velocity * speedVariation;
        }

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

        positionData.push_back(p.position.x);
        positionData.push_back(p.position.y);
        positionData.push_back(p.position.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instance);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positionData.size() * sizeof(float), positionData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj) {
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

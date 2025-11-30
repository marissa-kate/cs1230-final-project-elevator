#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "utils/scenedata.h"

class ParticleSystem {
public:
    // settings from constructor
    ParticleSystem(const SceneParticleEmitter& config);
    ~ParticleSystem();

    void init();
    void update(float deltaTime, float audioLevel, float audioFreq);
    void draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj);

private:
    SceneParticleEmitter m_config; // setting data

    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec4 color;
        float life;
        float phase; // sparkle
    };
    std::vector<Particle> m_particles;

    GLuint m_vao;
    GLuint m_vbo_mesh;
    GLuint m_vbo_instance;

    // helper
    float frand() { return (float)rand() / RAND_MAX; }
};

#include "inputs.h"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include <glm/gtc/quaternion.hpp>

/**
 * @brief getPrimitiveFromType
 * @param type
 * @return
 */
static Primitive* getPrimitiveFromType(PrimitiveType type) {
    // These objects exist for the lifetime of the program
    static Sphere sphere;
    static Cube cube;
    static Cylinder cylinder;
    static Cone cone;

    switch(type) {
    case PrimitiveType::PRIMITIVE_SPHERE:   return &sphere;
    case PrimitiveType::PRIMITIVE_CUBE:     return &cube;
    case PrimitiveType::PRIMITIVE_CYLINDER: return &cylinder;
    case PrimitiveType::PRIMITIVE_CONE:     return &cone;
    default: return nullptr;
    }
}

void Inputs::initialize(const RenderData &metaData){
    globalData = metaData.globalData;
    camera.init(metaData.cameraData);
    shapes = metaData.shapes;
    lights = metaData.lights;

    for (RenderShapeData &shape : shapes)
    {
        shape.rb = new RigidBody();
        RigidBody &rb = *shape.rb;

        rb.mass = 1.0f;
        rb.invMass = 1.0f / rb.mass;

        glm::vec3 scale(
            glm::length(glm::vec3(shape.ctm[0])),
            glm::length(glm::vec3(shape.ctm[1])),
            glm::length(glm::vec3(shape.ctm[2]))
            );
        rb.scale = scale;
        PrimitiveType t = shape.primitive.type;

        Primitive *prim = getPrimitiveFromType(t);
        rb.I_body = prim->inertiaTensor(rb.mass, scale);
        rb.I_body_inv = glm::inverse(rb.I_body);

        // translation from CTM

        rb.I_body_inv = glm::inverse(rb.I_body);

        // Position
        rb.x = glm::vec3(shape.ctm[3]);

        // Orientation
        rb.q = glm::quat_cast(shape.ctm);

        // Zero velocities
        rb.P = glm::vec3(0);
        rb.L = glm::vec3(0);
        rb.v = glm::vec3(0);
        rb.w = glm::vec3(0);

        // Force/torque accumulators
        rb.force = glm::vec3(0);
        rb.torque = glm::vec3(0);
    }

}

std::vector<RenderShapeData> & Inputs::getPrimitives () {
    return shapes;
};

const std::vector<SceneLightData> Inputs::getLights () const{
    return lights;
};
float getFloatFromLightType(LightType type){
    switch(type){
    case LightType::LIGHT_DIRECTIONAL: return 0.0; break;
    case LightType::LIGHT_POINT: return 1.0; break;
    case LightType::LIGHT_SPOT: return 2.0; break;
    }
    return 0.0f;  // default
}

void Inputs::sendLightUniformData(GLuint m_shader){
    //makeCurrent();
    int numLight = 0;
    for(SceneLightData light:lights){
        if(numLight<8){
            std::string posLoc = "m_lightPos[" + std::to_string(numLight) + "]";
            glUniform4fv(glGetUniformLocation(m_shader, posLoc.c_str()), 1,
                         &light.pos[0]);
            glUniform4fv(glGetUniformLocation(m_shader, ("m_lightDir[" + std::to_string(numLight) + "]").c_str()), 1,
                         &light.dir[0]);
            glUniform1f(glGetUniformLocation(m_shader, ("m_lightAngle[" + std::to_string(numLight) + "]").c_str()),
                        light.angle);
            glUniform4fv(glGetUniformLocation(m_shader, ("m_lightColor[" + std::to_string(numLight) + "]").c_str()),1,
                         &light.color[0]);
            glUniform1f(glGetUniformLocation(m_shader, ("m_lightPenumbra[" + std::to_string(numLight) + "]").c_str()),
                        light.penumbra);
            glUniform3fv(glGetUniformLocation(m_shader, ("m_lightFunction[" + std::to_string(numLight) + "]").c_str()),1,
                         &light.function[0]);
            glUniform1f(glGetUniformLocation(m_shader, ("m_lightType[" + std::to_string(numLight) + "]").c_str()),
                        getFloatFromLightType(light.type));
        }
        numLight++;
    }
    glUniform1i(glGetUniformLocation(m_shader, "numLight"), numLight);
}

void Inputs::sendGlobalUniformData(GLuint m_shader){
    glUniform1f(glGetUniformLocation(m_shader, "m_ka"), globalData.ka);
    glUniform1f(glGetUniformLocation(m_shader, "m_kd"), globalData.kd);
    glUniform1f(glGetUniformLocation(m_shader, "m_ks"), globalData.ks);
}

const SceneGlobalData& Inputs::getGlobalData() const {
    return globalData;
}

const Camera& Inputs::getCamera() const {
    return camera;
}

std::vector<float> Inputs::generateVBO(PrimitiveType type, int param1, int param2){
    Primitive *prim = getPrimitiveFromType(type);
    prim->updateParams(param1, param2);
    std::vector<float> vertexData = prim->generateShape();
    return vertexData;
}

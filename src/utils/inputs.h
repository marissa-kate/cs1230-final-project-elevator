#pragma once
#include "camera/camera.h"
#include "utils/sceneparser.h"
#include <GL/glew.h>
#define INPUTS_H

class Inputs
{
public:
    void sendLightUniformData(GLuint m_shader);
    void sendGlobalUniformData(GLuint m_shader);
    void initialize(const RenderData &metadata);
    std::vector<float> generateVBO(PrimitiveType type, int param1, int param2);
    std::vector<RenderShapeData> &getPrimitives();

    //getter of lights
    const std::vector<SceneLightData> getLights ()const ;
    // The getter of the width of the scene
    const int& width() const;
    // The getter of the height of the scene
    const int& height() const;

    // The getter of the global data of the scene
    const SceneGlobalData& getGlobalData() const;

    // The getter of the shared pointer to the camera instance of the scene
    const Camera& getCamera() const;

private:
    SceneGlobalData globalData;
    Camera camera;
    std::vector<RenderShapeData> shapes;
    std::vector<SceneLightData> lights;
};

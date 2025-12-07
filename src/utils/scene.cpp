#include "scene.h"
#include "camera/camera.h"
#include <stdexcept>
#include "utils/sceneparser.h"

/**
 * @brief RayTraceScene::RayTraceScene stores data from the scene for the ray tracer
 * @param width
 * @param height
 * @param metaData
 */
Scene::Scene(int width, int height, const RenderData &metaData) {
    sceneHeight = height;
    sceneWidth = width;
    globalData = metaData.globalData;
    camera.init(metaData.cameraData);
    shapes = metaData.shapes;
    lights = metaData.lights;
}

const std::vector<RenderShapeData> Scene::getPrimitives () const{
    return shapes;
};
const std::vector<SceneLightData> Scene::getLights () const{
    return lights;
};

const int& Scene::width() const{
    return sceneWidth;
}

const int& Scene::height() const {
    return sceneHeight;
}

const SceneGlobalData& Scene::getGlobalData() const {
    return globalData;
}

const Camera& Scene::getCamera() const {
    return camera;
}



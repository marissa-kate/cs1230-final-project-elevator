#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>
#include "utils/scenedata.h"
#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();
    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();
    glm::mat4 ctm = glm::mat4(1.0f);
    traverse(root, ctm, renderData);
    return true;
}

void SceneParser::traverse(SceneNode* node,
                           glm::mat4 cumulativeMatrix,
                           RenderData& renderData)
{
    // Step 1: apply this node's transformations
    for (SceneTransformation* t : node->transformations) {
        switch(t->type){
        case TransformationType::TRANSFORMATION_TRANSLATE:
            cumulativeMatrix*=glm::translate(t->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            cumulativeMatrix*=glm::scale(t->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            cumulativeMatrix*=glm::rotate(t->angle,t->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            cumulativeMatrix*=t->matrix;
            break;
        default:
            break;
        }
    }
    // Step 2: add lights
    for (SceneLight* light : node->lights) {
        SceneLightData newLight;
        newLight.id = light->id;
        newLight.color = light->color;
        newLight.type = light->type;
        newLight.function = light->function;
        newLight.dir = cumulativeMatrix*light->dir;
        newLight.pos = cumulativeMatrix* glm::vec4(0,0,0,1);
        newLight.angle = light->angle;
        newLight.penumbra = light->penumbra;
        renderData.lights.push_back(newLight);
    }

    // Step 3: add primitives
    for (ScenePrimitive* primitive : node->primitives) {
        RenderShapeData newShape;
        newShape.ctm = cumulativeMatrix;
        newShape.inverse_ctm = glm::inverse(cumulativeMatrix);
        newShape.primitive = *primitive;
        newShape.primitive.material = primitive->material;
        renderData.shapes.push_back(newShape);

    }

    for (SceneParticleEmitter* emitter : node->particles) {
        SceneParticleEmitter newEmitter = *emitter;

        glm::vec4 worldPos = cumulativeMatrix * glm::vec4(emitter->position, 1.0f);
        newEmitter.position = glm::vec3(worldPos);

        renderData.particles.push_back(newEmitter);
    }

    // Step 4: recurse to children
    for (SceneNode* child : node->children) {
        traverse(child, cumulativeMatrix, renderData);
    }
}

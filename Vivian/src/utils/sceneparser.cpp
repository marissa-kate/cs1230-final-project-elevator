#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    //populate renderData with global and camera data

    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();

    traverse(root, glm::mat4(1.0f), renderData);

    return true;
}

//helper recursive function
void SceneParser::traverse(SceneNode* node, glm::mat4 ctm, RenderData &renderData) {


    //store transforms in ctm
    for (SceneTransformation* t : node->transformations) {

        if(t->type == TransformationType::TRANSFORMATION_TRANSLATE){
            ctm = glm::translate(ctm, t-> translate);
        }else if(t->type == TransformationType::TRANSFORMATION_ROTATE){
            if(glm::length(t->rotate) > 0.0f){
                ctm = glm::rotate(ctm, t->angle, t->rotate);
            }
        }else if(t->type == TransformationType::TRANSFORMATION_SCALE){
            ctm = glm::scale(ctm, t->scale);
        }else if (t->type == TransformationType::TRANSFORMATION_MATRIX){
            ctm = ctm * t->matrix;
        }
    }

    //add shape to scene primitive
    for(ScenePrimitive *p : node->primitives){
        RenderShapeData shapeData;
        shapeData.primitive = *p;
        shapeData.ctm = ctm;

        shapeData.inv_ctm = glm::inverse(ctm);

        renderData.shapes.push_back(shapeData);
    }

    //add light to scenelight
    for(SceneLight *l : node->lights){
        SceneLightData lightData;

        //transfer light data
        lightData.id = l->id;
        lightData.type = l->type;
        lightData.color = l->color;
        lightData.function = l->function;
        lightData.penumbra = l->penumbra;
        lightData.angle = l->angle;


        //pos and dir matrices to world space
        lightData.pos = ctm * glm::vec4(0,0,0,1);
        lightData.dir = glm::normalize(ctm * l->dir);

        // float width, height;
        lightData.width = l->width;
        lightData.height = l->height;

        renderData.lights.push_back(lightData);
    }

    //recursion
    for (SceneNode* child : node->children) {
        traverse(child, ctm, renderData);
    }

}

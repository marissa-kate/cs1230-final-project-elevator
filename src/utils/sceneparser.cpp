#include "sceneparser.h"
#include "scenefilereader.h"
#include "scenedata.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <iostream>

struct Word {
    std::string word;
    Word* left;
    Word* right;
};

void dfsPrintTree(Word* w, std::string sentence) {
    // Task 4: Debug this function!!! (Hint: you may need to write/add some logic...)
    // Debug: The error was not considering if the node was at the edge.
    // I added this base case to check for null pointers
    if (w == nullptr) {
        return;
    }
    std::string newSentence = sentence + w->word ;
    if (w->left == nullptr && w->right == nullptr) {
        std::cout << newSentence << std::endl;
    }
    dfsPrintTree(w->left, newSentence);
    dfsPrintTree(w->right, newSentence);
}

Word* initTree(std::vector<Word> &words) {
    // STUDENTS - DO NOT TOUCH THIS FUNCTION
    words.reserve(8);
    words.push_back(Word{"2D graphics ", nullptr, nullptr});
    words.push_back(Word{"3D graphics ", nullptr, nullptr});
    words.push_back(Word{"making ", &words[0], &words[1]});
    words.push_back(Word{"CS123 ", nullptr, nullptr});
    words.push_back(Word{"love ", &words[3], &words[2]});
    words.push_back(Word{"bugs ", nullptr, nullptr});
    words.push_back(Word{"hate ", nullptr, &words[5]});
    words.push_back(Word{"I ", &words[4], &words[6]});
    return &words[7];
}

void SceneParser::debugDFS() {
    // Task 4: Uncomment this function
    std::vector<Word> words;
    Word* root = initTree(words);
    std::string sentence = "";
    dfsPrintTree(root, sentence);
}

// Helper Function for Task 6!
void SceneParser::dfsTraversal(SceneNode* node, glm::mat4 ctm, RenderData &renderData){
    //make a transformation matrix for each node
    glm::mat4 localTransform(1.0f);
    for (SceneTransformation* transformation : node->transformations){
        switch (transformation->type){
        case TransformationType::TRANSFORMATION_TRANSLATE:
            localTransform *= glm::translate(transformation->translate);
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            localTransform *= glm::scale(transformation->scale);
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            localTransform *= glm::rotate(transformation->angle, transformation->rotate);
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            localTransform *= transformation->matrix;
            break;

        }
    }
    //multiply parent to child
    glm::mat4 finalCtm = ctm * localTransform;

    for (ScenePrimitive* primitive : node->primitives){
        RenderShapeData shape;
        shape.primitive = *primitive;
        shape.ctm = finalCtm;
        renderData.shapes.push_back(shape);
    }

    //getting lights
    for (SceneLight* light : node->lights) {
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.type = light->type;
        lightData.color = light->color;
        lightData.function = light->function;
        lightData.penumbra = light->penumbra;
        lightData.angle = light->angle;

        lightData.pos = finalCtm * glm::vec4(0, 0, 0, 1);
        lightData.dir = finalCtm * light->dir;

        renderData.lights.push_back(lightData);
    }

    for (SceneParticleEmitter* emitter : node->particles) {
        glm::vec4 worldPos = finalCtm * glm::vec4(emitter->position, 1.0f);

        // save to renderdata
        SceneParticleEmitter dataCopy = *emitter;
        dataCopy.position = glm::vec3(worldPos);

        renderData.particles.push_back(dataCopy);
    }



    //do the function for the next child
    for (SceneNode* child : node->children){
        dfsTraversal(child, finalCtm, renderData);
    }
}



bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();


    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    renderData.shapes.clear();
    renderData.particles.clear();

    SceneNode* root = fileReader.getRootNode();
    if (root) {
        dfsTraversal(root, glm::mat4(1.0f), renderData);
    }

    return true;
}

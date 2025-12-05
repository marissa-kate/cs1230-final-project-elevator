#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

#include <string>
#include <sstream>

std::string primitiveTypeToString(PrimitiveType type) {
    switch (type) {
    case PrimitiveType::PRIMITIVE_CUBE: return "Cube";
    case PrimitiveType::PRIMITIVE_CONE: return "Cone";
    case PrimitiveType::PRIMITIVE_CYLINDER: return "Cylinder";
    case PrimitiveType::PRIMITIVE_SPHERE: return "Sphere";
    case PrimitiveType::PRIMITIVE_MESH: return "Mesh";
    default: return "Unknown";
    }
}

std::string materialToString(const SceneMaterial& mat) {
    std::ostringstream ss;
    ss << "Ambient: (" << mat.cAmbient.r << ", " << mat.cAmbient.g << ", " << mat.cAmbient.b << ", " << mat.cAmbient.a << ") "
       << "Diffuse: (" << mat.cDiffuse.r << ", " << mat.cDiffuse.g << ", " << mat.cDiffuse.b << ", " << mat.cDiffuse.a << ") "
       << "Specular: (" << mat.cSpecular.r << ", " << mat.cSpecular.g << ", " << mat.cSpecular.b << ", " << mat.cSpecular.a << ") "
       << "Shininess: " << mat.shininess;
    return ss.str();
}

std::vector<PrimitiveType> SceneParser::shape_type_helper(std::vector<RenderShapeData> shape_data) {
    std::vector<PrimitiveType> shape_type_data;
    for (int i = 0; i < shape_data.size(); ++i ) {
        PrimitiveType p = shape_data[i].primitive.type;
        if (std::find(shape_type_data.begin(), shape_type_data.end(), p ) == shape_type_data.end()) {
            shape_type_data.push_back(p);
        }
    }
    return shape_type_data;
}

void SceneParser::parseHelper(SceneNode *scenenode, RenderData &renderData, glm::mat4 cumulative_transformation_matrix) {
    std::vector<ScenePrimitive*> primitives_vector = scenenode->primitives;
    std::vector<SceneTransformation*> transformation_vector = scenenode->transformations;
    std::vector<SceneLight*> light_vector = scenenode->lights;
    for (int i = 0; i < transformation_vector.size(); i++) {
        // potentially, calcuatle firs th te ctm then apply to the primitive.
        SceneTransformation* t = transformation_vector.at(i);
        if (t->type == TransformationType::TRANSFORMATION_TRANSLATE) {
            glm::mat4 translate_matrix = glm::translate(t->translate);
            cumulative_transformation_matrix = cumulative_transformation_matrix * translate_matrix;
        } else if (t->type == TransformationType::TRANSFORMATION_SCALE) {
            glm::mat4 scale_matrix = glm::scale(t->scale);
            cumulative_transformation_matrix = cumulative_transformation_matrix * scale_matrix;
        } else if (t->type == TransformationType::TRANSFORMATION_ROTATE) {
            glm::mat4 rotate_matrix = glm::rotate(t->angle, t->rotate);
            cumulative_transformation_matrix = cumulative_transformation_matrix * rotate_matrix;
        } else if (t->type == TransformationType::TRANSFORMATION_MATRIX) {
            glm::mat4 scene_matrix = t->matrix;
            cumulative_transformation_matrix = cumulative_transformation_matrix * scene_matrix;
        }
    }
    for (int i = 0; i < primitives_vector.size(); i++) {
        RenderShapeData shape = RenderShapeData();
        shape.ctm = cumulative_transformation_matrix;
        ScenePrimitive* p = primitives_vector.at(i);
        std::string mat_str = primitiveTypeToString(p->type);
        SceneMaterial M = p->material;

        std::cout << renderData.globalData.ka << std::endl;
        std::cout << renderData.globalData.kd << std::endl;
        std::cout << renderData.globalData.ks << std::endl;
        std::cout << renderData.globalData.ks << std::endl;
        std::cout << mat_str << std::endl;
        std::cout << "ambient: " << glm::vec4(p->material.cAmbient).x << " " << glm::vec4(p->material.cAmbient).y << " " <<glm::vec4(p->material.cAmbient).z << std::endl;
        std::cout << "diffuse: " << glm::vec4(p->material.cDiffuse).x << " " << glm::vec4(p->material.cDiffuse).y << " " <<glm::vec4(p->material.cDiffuse).z << std::endl;
        std::cout << "specular: "<< glm::vec4(p->material.cSpecular).x << " " << glm::vec4(p->material.cSpecular).y << " " <<glm::vec4(p->material.cSpecular).z << std::endl;
        shape.primitive = *p;
        renderData.shapes.push_back(shape);
        glm::mat4 m = shape.ctm;
        // for (int i = 0; i < 4; ++i) {
        //     for (int j = 0; j < 4; ++j) {
        //         std::cout << m[i][j] << " ";
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << primitives_vector.size() <<std::endl;
    }
    for (int i = 0; i < light_vector.size(); i++) {
        SceneLightData light_data = SceneLightData();
        SceneLight* light = light_vector.at(i);
        light_data.id = light->id;
        light_data.type = light->type;
        light_data.color = light->color;
        light_data.function = light->function;
        light_data.pos = cumulative_transformation_matrix * glm::vec4(0.0, 0.0, 0.0, 1.0f);
        light_data.dir = cumulative_transformation_matrix * light->dir;
        light_data.penumbra = light->penumbra;
        light_data.angle = light->angle;
        light_data.width = light->width;
        light_data.height = light->height;
        renderData.lights.push_back(light_data);
    }
    std::vector<SceneNode*> children_nodes = scenenode->children;
    for (SceneNode* child : children_nodes) {
        parseHelper(child, renderData, cumulative_transformation_matrix);
    }


    // added this function to fll in primtive types:
    renderData.shape_types = shape_type_helper(renderData.shapes);
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    // Task 5: populate renderData with global data, and camera data;
    SceneGlobalData global_data = fileReader.getGlobalData();
    SceneCameraData camera_data = fileReader.getCameraData();
    renderData.globalData = global_data;
    renderData.cameraData = camera_data;

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    SceneNode* root_node = fileReader.getRootNode();
    renderData.shapes.clear();
    glm::mat4 cumulative_transformation_matrix = glm::mat4(1.0f);
    parseHelper(root_node, renderData, cumulative_transformation_matrix);
    return true;
}

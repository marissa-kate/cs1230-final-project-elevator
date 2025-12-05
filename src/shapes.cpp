#include "shapes.h"
#include <iostream>
#include <glm/glm.hpp>

#include "utils/sphere.h"
#include "utils/cube.h"
#include "utils/cylinder.h"
#include "utils/cone.h"
#include <QMouseEvent>
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

shapes::shapes() {}

std::vector<float> shapes::generate_shape_data(PrimitiveType shape_type, int param1, int param2) {
    if (shape_type == PrimitiveType::PRIMITIVE_SPHERE) {
        sphere sphere;
        return sphere.generateSphereData(param1, param2);
    } else if (shape_type == PrimitiveType::PRIMITIVE_CUBE) {
        cube cube;
        return cube.generateCubeData(param1, param2);
    } else if (shape_type == PrimitiveType::PRIMITIVE_CYLINDER) {
        cylinder cylinder;
        return cylinder.generateCylinderData(param1, param2);
    } else if (shape_type == PrimitiveType::PRIMITIVE_CONE) {
        cone cone;
        return cone.generateConeData(param1, param2);
    }
}

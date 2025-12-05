#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

// // from lab 8:
enum ShapeType {
    SHAPE_TRIANGLE,
    SHAPE_CUBE,
    SHAPE_CONE,
    SHAPE_SPHERE,
    SHAPE_CYLINDER,
    SHAPE_TETRAHEDRON
};
// // from lab 8 ^

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float aperature = 1;
    float focal_length = 1;
    float nearPlane = 1;
    float farPlane = 1;
    float focal_plane = 1;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;

    // // from lab 8:
    // bool showWireframeNormals = true;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGui/qimage.h>
#include <string>

struct Settings {
    std::string sceneFilePath;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float nearPlane = 0.1;
    float farPlane = 100;
    float exposure = 0.5;
    float bloomThreshold = 0.4;
    bool perPixelFilter = false;
    bool kernelBasedFilter = false;
    bool extraCredit1 = false;
    bool extraCredit2 = false;
    bool extraCredit3 = false;
    bool extraCredit4 = false;
    QImage bumpTexture;
    bool hasTexture = false;
    int bumpDepth = 10;
    bool invert = false;
    bool color = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H

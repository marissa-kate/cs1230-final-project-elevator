#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <QImage>

struct Settings {
    std::string sceneFilePath;
    // std::string bumpTextureFilePath;
    QImage bumpTexture;
    bool hasTexture = false;
    int bumpDepth = 10;
    int shapeParameter1 = 1;
    int shapeParameter2 = 1;
    float nearPlane = 1;
    float farPlane = 1;
    bool invert = false;
    bool color = false;

};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H

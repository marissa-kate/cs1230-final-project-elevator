cmake_minimum_required(VERSION 3.16)

# Sets project name
project(projects_realtime LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Xml)
find_package(Qt6 REQUIRED COMPONENTS Multimedia) # 追加: オーディオ機能用

# Allows you to include files from within those directories, without prefixing their filepaths
include_directories(src)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/realtime.cpp
    src/realtime.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/settings.cpp
    src/settings.h

    # Utils
    src/utils/scenefilereader.cpp
    src/utils/scenefilereader.h
    src/utils/sceneparser.cpp
    src/utils/sceneparser.h
    src/utils/scenedata.h
    src/utils/shaderloader.h
    src/utils/aspectratiowidget/aspectratiowidget.hpp

    # Physics
    src/utils/phys.cpp
    src/utils/phys.h
    src/utils/inputs.cpp
    src/utils/inputs.h

    # Shapes
    src/shapes/Primitive.h
    src/shapes/Cone.cpp src/shapes/Cone.h
    src/shapes/Cube.cpp src/shapes/Cube.h
    src/shapes/Cylinder.cpp src/shapes/Cylinder.h
    src/shapes/Sphere.cpp src/shapes/Sphere.h

    # Camera
    src/camera/camera.cpp src/camera/camera.h

    # Added Features (Particles, Audio, LSystem)
    src/audiocapture.cpp src/audiocapture.h
    src/particlesystem.cpp src/particlesystem.h
    src/lsystem.cpp src/lsystem.h
)

# GLM: this creates its library and allows you to `#include "glm/..."`
add_subdirectory(glm)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Xml
    Qt::Multimedia # added
    StaticGLEW
)

# Specifies other files (Shaders)
qt6_add_resources(${PROJECT_NAME} "Resources"
    PREFIX
        "/"
    FILES
        resources/shaders/default.frag
        resources/shaders/default.vert
        resources/shaders/light.frag
        resources/shaders/light.vert
        resources/shaders/dof.frag
        resources/shaders/dof.vert
        resources/shaders/color-grading.frag
        resources/shaders/color-grading.vert
        resources/shaders/texture.frag
        resources/shaders/final.vert
        resources/shaders/final.frag

        # Added Shaders
        resources/shaders/particle.vert
        resources/shaders/particle.frag
        resources/shaders/color-grading.vert
        resources/shaders/color-grading.frag
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()
# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()

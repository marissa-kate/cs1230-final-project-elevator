#pragma once

#include "utils/scenedata.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class CameraPath {
public:
    CameraPath();
    ~CameraPath();

    // set imported keyframe data
    void fromKeyframes(const std::vector<SceneCameraKeyframe>& keyframes);

    // calculate camera data at t based on that
    SceneCameraData evaluate(float t);

private:
    struct Frame {
        float time;
        glm::vec3 pos;
        glm::quat rot; // save rotation
    };
    std::vector<Frame> m_frames;

    // create bezier curve
    glm::vec3 evaluateBezierPosition(int segmentIndex, float localT);

    // create control points of the bezier curve
    struct BezierControlPoints {
        glm::vec3 p1; // start control
        glm::vec3 p2; // end control
    };
    std::vector<BezierControlPoints> m_controlPoints;

    void computeControlPoints();
};

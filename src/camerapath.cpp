#include "camerapath.h"
#include <algorithm>
#include <cmath>
#include <iostream>

CameraPath::CameraPath() {}
CameraPath::~CameraPath() {}

void CameraPath::fromKeyframes(const std::vector<SceneCameraKeyframe>& keyframes) {
    std::cout << "[DEBUG] CameraPath received " << keyframes.size() << " keyframes." << std::endl;
    m_frames.clear();
    if (keyframes.empty()) return;

    // change keyframe to quater union - apparently it's more smoother
    for (const auto& kf : keyframes) {
        Frame f;
        f.time = kf.time;
        f.pos = kf.position;

        // make quotar union from lookat
        if (glm::length(kf.look) > 0.0001f) {
            f.rot = glm::quatLookAt(glm::normalize(kf.look), kf.up);
        } else {
            f.rot = glm::quat(1, 0, 0, 0); // Identity
        }

        m_frames.push_back(f);
    }

    // 2. sort based on time
    std::sort(m_frames.begin(), m_frames.end(), [](const Frame& a, const Frame& b) {
        return a.time < b.time;
    });

    // 3. compute the bejier points
    computeControlPoints();
}

void CameraPath::computeControlPoints() {
    m_controlPoints.clear();
    if (m_frames.size() < 2) return;

    for (size_t i = 0; i < m_frames.size() - 1; ++i) {
        glm::vec3 p0 = m_frames[i].pos;
        glm::vec3 p3 = m_frames[i+1].pos;

        float dt = m_frames[i+1].time - m_frames[i].time;
        if (dt <= 0.0001f) {
            BezierControlPoints cp;
            cp.p1 = p0;
            cp.p2 = p3;
            m_controlPoints.push_back(cp);
            continue;
        }

        // Calculate the velocity

        // Start Velocity (v0)
        glm::vec3 v0;
        if (i == 0) {
            // origin
            v0 = (p3 - p0) / dt;
        } else {
            glm::vec3 prevP = m_frames[i-1].pos;
            float prevDt = m_frames[i].time - m_frames[i-1].time;
            // average speed based on previous points
            v0 = (p3 - prevP) / (dt + prevDt);
        }

        // End Velocity (v1)
        glm::vec3 v1;
        if (i == m_frames.size() - 2) {
            // end point
            v1 = (p3 - p0) / dt;
        } else {
            glm::vec3 nextP = m_frames[i+2].pos;
            float nextDt = m_frames[i+2].time - m_frames[i+1].time;
            v1 = (nextP - p0) / (dt + nextDt);
        }

        // change to bezier control points
        BezierControlPoints cp;
        cp.p1 = p0 + v0 * (dt / 3.0f);
        cp.p2 = p3 - v1 * (dt / 3.0f);

        m_controlPoints.push_back(cp);
    }
}

SceneCameraData CameraPath::evaluate(float t) { //time evaluation: in what keyframe is this movement at right now?
    SceneCameraData data;

    // default point
    if (m_frames.empty()) return data;

    // clamp
    if (t <= m_frames.front().time) {
        Frame& f = m_frames.front();
        data.pos = glm::vec4(f.pos, 1.0f);
        // quater union to look and up
        glm::vec3 look = f.rot * glm::vec3(0, 0, -1);
        glm::vec3 up = f.rot * glm::vec3(0, 1, 0);
        data.look = glm::vec4(look, 0.0f);
        data.up = glm::vec4(up, 0.0f);
        return data;
    }
    if (t >= m_frames.back().time) {
        Frame& f = m_frames.back();
        data.pos = glm::vec4(f.pos, 1.0f);
        glm::vec3 look = f.rot * glm::vec3(0, 0, -1);
        glm::vec3 up = f.rot * glm::vec3(0, 1, 0);
        data.look = glm::vec4(look, 0.0f);
        data.up = glm::vec4(up, 0.0f);
        return data;
    }

    // find segment based on it
    int segmentIndex = 0;
    for (size_t i = 0; i < m_frames.size() - 1; ++i) {
        if (t >= m_frames[i].time && t < m_frames[i+1].time) {
            segmentIndex = i;
            break;
        }
    }

    // calculate parameter u
    float t0 = m_frames[segmentIndex].time;
    float t1 = m_frames[segmentIndex + 1].time;
    float u = (t - t0) / (t1 - t0);

    // 1. cubic beizer
    glm::vec3 p0 = m_frames[segmentIndex].pos;
    glm::vec3 p3 = m_frames[segmentIndex+1].pos;
    glm::vec3 p1 = m_controlPoints[segmentIndex].p1;
    glm::vec3 p2 = m_controlPoints[segmentIndex].p2;

    // B(t) = (1-u)^3 P0 + 3(1-u)^2 u P1 + 3(1-u) u^2 P2 + u^3 P3 based on the article
    glm::vec3 pos = (float)pow(1-u, 3) * p0 +
                    3 * (float)pow(1-u, 2) * u * p1 +
                    3 * (1-u) * u * u * p2 +
                    u * u * u * p3;

    // 2. lerp rotation
    glm::quat q0 = m_frames[segmentIndex].rot;
    glm::quat q1 = m_frames[segmentIndex + 1].rot;
    glm::quat rot = glm::slerp(q0, q1, u);

    // 3. set to data
    data.pos = glm::vec4(pos, 1.0f);
    data.look = glm::vec4(rot * glm::vec3(0, 0, -1), 0.0f);
    data.up = glm::vec4(rot * glm::vec3(0, 1, 0), 0.0f);

    // Keeping the FOV for now? Maybe change in the future?
    data.heightAngle = glm::radians(45.0f);

    return data;
}

#include "lsystem.h"
#include <stack>
#include <cmath>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

LSystem::LSystem(const LSystemConfig& config) : m_config(config) {}

//simple lsystem generater
std::vector<Segment> LSystem::generate() {
    if (m_config.iterations <= 0) return {};

    std::string instruction = expandString();
    return processTurtle(instruction);
}

//procedural generation
std::string LSystem::expandString() {
    std::string current = m_config.axiom;
    bool hasX = (m_config.axiom.find('X') != std::string::npos);

    for (int i = 0; i < m_config.iterations; ++i) {
        std::string next = "";
        for (char c : current) {
            if (hasX) {
                // plant mode
                if (c == 'X') {
                    next += m_config.rule;
                } else if (c == 'F') {
                    next += "FF";
                } else {
                    next += c;
                }
            } else {
                // F only?
                if (c == 'F') {
                    next += m_config.rule;
                } else {
                    next += c;
                }
            }
        }
        current = next;
    }
    return current;
}

//convert output to positions
std::vector<Segment> LSystem::processTurtle(const std::string& instruction) {
    std::vector<Segment> segments;

    struct State {
        glm::vec3 pos;
        glm::vec3 dir; // direction
    };

    std::stack<State> stateStack;
    State currentState = { m_config.startPos, glm::vec3(0, 1, 0) }; // above

    // determining length based on for loop
    float currentLen = m_config.length / std::pow(1.5f, m_config.iterations - 1);
    float angleRad = glm::radians(m_config.angle);

    // twister
    glm::quat rotLeft = glm::angleAxis(angleRad, glm::vec3(0, 0, 1));  // Z axis twist
    glm::quat rotRight = glm::angleAxis(-angleRad, glm::vec3(0, 0, 1));

    for (char c : instruction) {//drawer
        if (c == 'F') {
            glm::vec3 nextPos = currentState.pos + (currentState.dir * currentLen);
            segments.push_back({currentState.pos, nextPos});
            currentState.pos = nextPos;
        }
        else if (c == '+') {
            currentState.dir = rotLeft * currentState.dir;
        }
        else if (c == '-') {
            currentState.dir = rotRight * currentState.dir;
        }
        else if (c == '[') {
            stateStack.push(currentState);
        }
        else if (c == ']') {
            if (!stateStack.empty()) {
                currentState = stateStack.top();
                stateStack.pop();
            }
        }
    }
    return segments;
}

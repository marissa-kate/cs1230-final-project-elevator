#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <map>

// line branch data
struct Segment {
    glm::vec3 start;
    glm::vec3 end;
};

// L-System settings
struct LSystemConfig {
    std::string axiom;      // starting F
    std::string rule;       // how does f get changed?
    int iterations;         // How many times do we do this
    float angle;            // how much does it branch
    float length;           // how long is the branch
    glm::vec3 startPos;     // where does it start from
};

class LSystem {
public:
    LSystem(const LSystemConfig& config);

    // create text and generate positions
    std::vector<Segment> generate();

private:
    LSystemConfig m_config;

    // opens string
    std::string expandString();

    // turtle graphics:make positions from strings!
    std::vector<Segment> processTurtle(const std::string& instruction);
};

#version 330 core

// Task 15: add a second layout variable representing a UV coordinate
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;


// Task 16: create an "out" variable representing a UV coordinate
out vec2 frag_uv;

// uniform vec2 u_scale;

void main() {
    // Task 16: assign the UV layout variable to the UV "out" variable
    frag_uv = uv;

    gl_Position = vec4(position, 1.0);

    // gl_Position = vec4(position.x * u_scale.x, position.y * u_scale.y, position.z, 1.0);
}

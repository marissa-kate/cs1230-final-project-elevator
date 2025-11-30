#version 330 core
layout (location = 0) in vec3 aPos; // where is the sphere
layout (location = 2) in vec3 aOffset; //for particle

uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec4 u_baseColor;
uniform float u_scale;

out vec2 v_texCoord; // uv cord for vertex shader
out vec4 v_color;
out float v_phase; //sparkle

float random(vec3 st) {
    return fract(sin(dot(st.xyz, vec3(12.9898, 78.233, 45.543))) * 43758.5453123);
}

void main() {
    // 1. particle place to view cord
    vec3 centerInView = (u_view * vec4(aOffset, 1.0)).xyz;

    // 2. flipping positions based on xy planes- quads always are towards the camera
    vec3 vertexInView = centerInView + (aPos * u_scale);

    // 3. to projection
    gl_Position = u_proj * vec4(vertexInView, 1.0);

    // 4. calculate uv
    v_texCoord = aPos.xy + 0.5; //for drawing

    v_phase = random(aOffset) * 6.28;

    v_color = u_baseColor; // water color
}

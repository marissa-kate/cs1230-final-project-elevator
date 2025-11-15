#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;


uniform mat4 u_model; // model positions
uniform mat4 u_view;  // camera position
uniform mat4 u_proj;  // lens setting

out vec3 v_worldNormal;
out vec3 v_worldPos;

out vec3 norm;

void main() {
   v_worldPos = vec3(u_model * vec4(position, 1.0));
   v_worldNormal = mat3(transpose(inverse(u_model))) * normal;
   gl_Position = u_proj * u_view * u_model * vec4(position, 1.0);
}

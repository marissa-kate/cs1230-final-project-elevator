#version 330 core
layout (location = 0) in vec3 full_screen_position;
layout (location = 1) in vec2 full_screen_uv;

out vec2 frag_uv;

void main() {
   gl_Position = vec4(full_screen_position, 1.0);
   frag_uv = full_screen_uv;
}

#version 330 core
in vec2 frag_uv;
out vec4 fragColor;
uniform sampler2D uBackground;

void main() {
    fragColor = texture(uBackground, frag_uv);
}

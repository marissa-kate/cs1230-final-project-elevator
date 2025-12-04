#version 330 core
layout(location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 pu;
layout (location = 4) in vec3 pv;

out vec3 world_pos;
out vec3 world_normal;
out vec2 frag_uv;
out vec3 frag_pu;
out vec3 frag_pv;


uniform mat3 normalMatrix;
uniform mat4 mvp;



void main() {

    frag_uv = uv;
    frag_pu = pu;
    frag_pv = pv;

    world_pos = vec3(mvp * vec4(position, 1.0));
    world_normal = normalize(normalMatrix * normal);
    gl_Position = mvp * vec4(position, 1.0);




}

#version 330 core
layout (location = 0) in vec3 os_pos;
layout(location = 1) in vec3 os_norm;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 pu;
layout (location = 4) in vec3 pv;

out vec3 ws_pos;
out vec3 ws_norm;
out vec3 es_pos;
out vec2 frag_uv;
out vec3 frag_pu;
out vec3 frag_pv;

uniform mat4 m_model;
uniform mat4 m_view;
uniform mat4 m_proj;

void main() {
    frag_uv = uv;
    frag_pu = pu;
    frag_pv = pv;

    ws_pos = vec3(m_model * vec4(os_pos, 1.0));
    ws_norm = normalize(mat3(transpose(inverse(m_model))) * os_norm);
    vec4 pos_eye = m_view * m_model * vec4(os_pos, 1.0);
    es_pos = pos_eye.xyz;
    gl_Position = m_proj * pos_eye;

}

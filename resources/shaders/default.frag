#version 330 core
in vec3 ws_pos;
in vec3 ws_norm;
in vec3 es_pos;
in vec2 frag_uv;
in vec3 frag_pu;
in vec3 frag_pv;

layout( location = 2) out vec4 out_position;
layout( location = 3) out vec4 out_normal;
layout( location = 4) out vec4 out_ambient_shininess;
layout( location = 5) out vec4 out_albedo_spec;
layout( location = 6) out float depth_out;

uniform vec4 cAmbient;
uniform vec4 cDiffuse;
uniform vec4 cSpecular;
uniform float m_shininess;

uniform sampler2D u_texture;
uniform bool hasTexture;
uniform int bump_depth;

void main() {
    vec3 normal = vec3(normalize(ws_norm));

    if (hasTexture){
            float texture_val = texture(u_texture, frag_uv).r;

            int limit = 10;

            float texture_func = 10 * texture_val - 5; //between -5 and 5

            ivec2 size = textureSize(u_texture, 0);
            float textureWidth  = size.x;
            float textureHeight = size.y;

            float du = 1.0f / textureWidth;
            float dv = 1.0 / textureHeight;


            float h = texture(u_texture, frag_uv).r;
            float hU = texture(u_texture, frag_uv + vec2(du, 0)).r;
            float hV = texture(u_texture, frag_uv + vec2(0, dv)).r;

            float Fu = hU - h;
            float Fv = hV - h;

            Fu *= bump_depth;
            Fv *= bump_depth;

            // ** before bumpSlider has been implemented, just use a constant
            // Fu = texture(u_texture, frag_uv).r * 10;
            // Fv = texture(u_texture, frag_uv).r * 10;

            //updated bump normal
            normal = normalize(normal + Fu * cross(normal, frag_pv) + Fv * cross(frag_pu, normal));

            // ** before implementing bump, try to output textures directly
            // vec4 color = vec4(texture(u_texture, frag_uv).r, texture(u_texture, frag_uv).r, texture(u_texture, frag_uv).r, 1);

            // fragColor = color;

        }

    out_normal = vec4(normal, 0.0f);
    out_position = vec4(ws_pos, 1.0f);
    out_ambient_shininess = vec4(cAmbient.xyz, m_shininess);
    out_albedo_spec = vec4(cDiffuse.xyz, cSpecular.x);
    depth_out = gl_FragCoord.z;
}

#version 330 core

in vec2 frag_uv;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

//lighting uniforms
uniform vec4 camera_pos;
uniform float m_ka;
uniform float m_kd;
uniform float m_ks;


uniform int numLight;

uniform mat4 m_view;

uniform sampler2D color_position;
uniform sampler2D color_normal;
uniform sampler2D ambient_shininess;
uniform sampler2D albedo_spec;
uniform sampler2D depth;
uniform sampler2D final_color;

uniform vec4 m_lightPos[8];
uniform vec4 m_lightDir[8];
uniform vec3 m_lightFunction[8];
uniform vec4 m_lightColor[8];
uniform float m_lightType[8];
uniform float m_lightAngle[8];
uniform float m_lightPenumbra[8];

//fog uniforms
uniform float fog_minDist;
uniform float fog_maxDist;
uniform vec4  fog_color;

uniform float bloomThreshold;

void main() {

    vec3 normal = normalize(texture(color_normal, frag_uv)).xyz;
    vec4 ambient_shine = texture(ambient_shininess, frag_uv);
    vec4 diffuse_spec = texture(albedo_spec, frag_uv);

    vec4 position = texture(color_position, frag_uv);
    vec4 es_pos = m_view * position;
    vec3 ws_pos = position.xyz;

    vec4 cAmbient = vec4(ambient_shine.xyz, 1.0f);
    vec4 cDiffuse = vec4(diffuse_spec.xyz, 0.0f);
    vec4 cSpecular = vec4(vec3(diffuse_spec.w), 0.0f);
    float m_shininess = ambient_shine.w;

    fragColor = m_ka * cAmbient;
     for(int i = 0; i < numLight; i ++){
         vec4 color = vec4(0, 0, 0, 1);
         vec3 lightdir;
         float f_att = 1.0; //Attenuation
         float intensity = 1.0; //Intensity (varies for spot lights)

        if(m_lightType[i] == 0){ //dir
            lightdir = normalize(vec3(-m_lightDir[i]));
        }

        else if(m_lightType[i] == 1){ // point
            lightdir = normalize(vec3(m_lightPos[i])-ws_pos);
            float dist = distance(vec3(m_lightPos[i]), ws_pos);
            f_att = min(1.0, 1 / (m_lightFunction[i][0] + dist * m_lightFunction[i][1] +
                                       dist * dist * m_lightFunction[i][2]));
        }
        else if(m_lightType[i] == 2){ //spot
            lightdir = normalize(-vec3(m_lightDir[i]));
            vec3 pointdir = normalize(vec3(m_lightPos[i]) - ws_pos);
            float theta = acos(dot(lightdir, pointdir));
            float inner = m_lightAngle[i] - m_lightPenumbra[i];
            float outer = m_lightAngle[i];
            if(theta > inner && theta <= outer){ //falloff
                float falloff = (-2 * pow((theta - inner) / (outer - inner), 3)) +
                                (3 * pow((theta - inner) / (outer - inner), 2));
                intensity = 1.0 - falloff;
            } else if(theta > outer) { //outside of cone of light
                intensity = 0.0;
            }
            float dist = distance(vec3(m_lightPos[i]), ws_pos);
            f_att = min(1.0, 1 / (m_lightFunction[i][0] + dist * m_lightFunction[i][1] +
                                       dist * dist * m_lightFunction[i][2]));
        }

      //Diffuse color & texture color are added here
         float lambert = max(dot(normal, normalize(lightdir)), 0.0);
         vec4 diffuse =  m_kd * m_lightColor[i] * cDiffuse * lambert * intensity * f_att;
         fragColor += diffuse;

    // if(lambert>0.0){
        vec3 reflected = normalize(reflect(lightdir, normal));
        float specDot = max(dot(reflected, (normalize(ws_pos-vec3(camera_pos)))), 0.0);
        float shiny = pow(specDot, m_shininess);
        fragColor +=  m_ks * m_lightColor[i] * cSpecular * shiny;
    }

     // -------- FOG ---------
     float dist = length(es_pos);
     float fogFactor = (fog_maxDist - dist) / (fog_maxDist - fog_minDist);
     fogFactor = clamp(fogFactor, 0.0, 1.0);

     // blend between fog and shaded color
     fragColor = mix(fog_color, fragColor, fogFactor);

     float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
         if(brightness > bloomThreshold)
             brightColor = vec4(fragColor.rgb, 1.0);
         else
             brightColor = vec4(0.0, 0.0, 0.0, 1.0);


}

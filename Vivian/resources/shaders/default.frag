#version 330 core

in vec3 world_pos;
in vec3 world_normal;
in vec2 frag_uv;
in vec3 frag_pu;
in vec3 frag_pv;


out vec4 fragColor;

uniform sampler2D u_texture;
uniform bool hasTexture;

uniform float k_a;
uniform float k_d;
uniform float k_s;
uniform vec4 world_camera;
uniform int num_lights;

uniform int bump_depth;

struct SceneLight{
    int type; //0 = point, 1 = directional, 2 = spot
    vec3 color;
    vec3 function;
    vec4 pos;
    vec4 dir;
    float penumbra;
    float angle;
};

struct Material{
    vec3 cAmbient;
    vec3 cDiffuse;
    vec3 cSpecular;
    float shininess;
};

uniform SceneLight lights[20];
uniform Material material;

void main() {

    vec3 N = normalize(world_normal);


    //bumpMapping

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
        N = normalize( N + Fu * cross(N, frag_pv) + Fv * cross(frag_pu, N));

        // ** before implementing bump, try to output textures directly
        // vec4 color = vec4(texture(u_texture, frag_uv).r, texture(u_texture, frag_uv).r, texture(u_texture, frag_uv).r, 1);

        // fragColor = color;

    }
    // Remember that you need to renormalize vectors here if you want them to be normalized

    vec3 ambient= material.cAmbient * k_a;
    fragColor = vec4(ambient, 1);

    //loop through the lights
    for (int i = 0; i < num_lights; i++) {

        vec3 diffuse = material.cDiffuse * k_d;
        vec3 specular = material.cSpecular * k_s;

        vec3 d = vec3(lights[i].pos) - world_pos;
        float distance = sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
        float fatt = lights[i].function.x + lights[i].function.y * distance + lights[i].function.z * distance * distance;
        fatt = 1.0 / max(fatt, 0.1);



        if (lights[i].type == 0) { // point light

            vec3 L = normalize(vec3(lights[i].pos) - world_pos);
            // vec3 N = normalize(world_normal);

            float direction = max(dot(N, L), 0.0);

            vec3 R = normalize(reflect(-L, N));
            vec3 E = normalize(vec3(world_camera) - world_pos);

            float spec = max(dot(R, E), 0.0001);
            diffuse = diffuse * direction;
            specular = vec3(specular * pow(spec, max(material.shininess, 1.0)));

            fragColor += vec4(lights[i].color * fatt * (diffuse + specular), 1.0);

        } else if (lights[i].type == 1) { //directional

            vec3 L = normalize(vec3(-lights[i].dir));
            // vec3 N = normalize(world_normal);

            float direction = max(dot(N, L), 0.0);
            // diffuse = diffuse * direction;

            //add specular component to output color
            vec3 R = normalize(reflect(-L, N));
            vec3 E = normalize(vec3(world_camera) - world_pos);

            float spec = max(dot(R, E), 0.1);
            // specular = vec3(specular * pow(spec, material.shininess));

            fragColor += vec4(lights[i].color * (diffuse * direction + specular * pow(spec, max(material.shininess, 1.0))), 1.0);

        } else if (lights[i].type == 2) { // spot

            float intensity = 0.0;

            vec3 L = normalize(vec3(lights[i].pos) - world_pos);
            // vec3 N = normalize(world_normal);

            float direction = max(dot(N, L), 0.0);


            vec3 R = normalize(reflect(-L, N));
            vec3 E = normalize(vec3(world_camera) - world_pos);

            diffuse = diffuse * direction;
            float spec = max(dot(R, E), 0.1);
            specular = vec3(specular * pow(spec, max(material.shininess, 1.0)));

            vec3 lightDir = normalize(vec3(-lights[i].dir));
            float cosTheta = dot(L, lightDir);

            float angle = acos(cosTheta);
            float angle_inner = lights[i].angle - lights[i].penumbra;

            if(angle < lights[i].angle){ //then intersection is within the outer cone
                if(angle < angle_inner){ //if intersection ray is within inner cone

                    intensity = 1.0f;

                } else {  //spot falloff calculation

                    float t = (angle - angle_inner) / lights[i].penumbra;
                    t = max(t, 0.0);
                    t = min(t, 1.0);
                    intensity = 1.0f - ( - 2.0f*t*t*t + 3.0f*t*t);
                    // intensity = 0.5f;
                }
            }
            fragColor += vec4(lights[i].color * fatt * intensity * (diffuse + specular), 1.0);
        }
    }


    // color corresponding to normal values, you can try this as well with the updated bump normals
    // fragColor = vec4((N + 1.0f) / 2, 1);


}






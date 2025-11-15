#version 330 core
in vec3 v_worldNormal;
in vec3 v_worldPos;
out vec4 fragColor;

//scemenmaterial
struct SceneMaterial {
    vec4 cAmbient;
    vec4 cDiffuse;
    vec4 cSpecular;
    float shininess;
};

uniform SceneMaterial u_material;

//scenelightdata
struct SceneLightData {
    int type;      // 0=Point, 1=Directional
    vec4 color;
    vec3 function;
    vec4 pos;
    vec4 dir;
};

uniform SceneLightData u_lights[10]; //max 10 for now
uniform int u_numLights;

//sceneglobaldata
struct SceneGlobalData {
    float ka;
    float kd;
    float ks;
};
uniform SceneGlobalData u_global;

//camera
uniform vec3 u_camPos;

//calculate light
vec3 calculateLight(SceneLightData light, vec3 N, vec3 V) {
    vec3 L;
    float atten = 1.0;

    if (light.type == 0) { // Point Light
        L = normalize(light.pos.xyz - v_worldPos);
        float dist = length(light.pos.xyz - v_worldPos);
        atten = 1.0 / (light.function.x + light.function.y * dist + light.function.z * dist * dist);
    } else { // Directional Light
        L = normalize(-light.dir.xyz);
        atten = 1.0;
    }

    // Diffuse
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = u_global.kd * u_material.cDiffuse.rgb * NdotL;

    // Specular
    vec3 R = reflect(-L, N);
    float RdotV = max(dot(R, V), 0.0);
    vec3 specular = u_global.ks * u_material.cSpecular.rgb * pow(RdotV, u_material.shininess);

    return atten * light.color.rgb * (diffuse + specular);
}


void main() {

    // 1. normalize
    vec3 N = normalize(v_worldNormal);
    vec3 V = normalize(u_camPos - v_worldPos);

    // 2. Ambient
    vec3 ambient = u_global.ka * u_material.cAmbient.rgb;

    // 3. add diffuse and specuilar
    vec3 totalLighting = vec3(0.0);
    for (int i = 0; i < u_numLights; i++) {
        totalLighting += calculateLight(u_lights[i], N, V);
    }

    // 4. sum total color
    fragColor = vec4(ambient + totalLighting, 1.0);
}

#version 330 core
in vec2 v_texCoord;
in vec4 v_color;
in float v_phase;

uniform float u_time; //from c++
out vec4 fragColor;

//simple!!!!
void main() {
    // distance
    float dist = distance(v_texCoord, vec2(0.5));

    // change opacity based on distance
    float alpha = 1.0 - smoothstep(0.0, 0.5, dist);
    float sparkle = 0.6 + 0.4 * sin(u_time * 5.0 + v_phase);

    // strong color
    fragColor = vec4(v_color.rgb, alpha * v_color.a * sparkle);

    // dont draw transparent
    if (fragColor.a < 0.01) discard;
}

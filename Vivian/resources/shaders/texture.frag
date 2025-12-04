#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 frag_uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D u_texture;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool bool_texture;

uniform int filter_type; // 0: invert, 1: color

out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    // // Task 17: Set fragColor using the sampler2D at the UV coordinate
    vec4 color = vec4(texture(u_texture, frag_uv));

    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if(bool_texture){

        if(filter_type == 0){

        color.rgb = 1.0 - color.rgb;
        // color = vec4(1.0, 0, 0, 1.0);
        }

        if(filter_type == 1){

            color.rgb = vec3(1.0, 0, 0);

        }

    } else{

        color.rgb = color.rgb;
    }



    fragColor = color;
}

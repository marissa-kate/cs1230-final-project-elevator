#version 330 core
// Task 16: Create a UV coordinate in variable
in vec2 frag_uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D u_texture;

uniform sampler3D lut;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool bool_texture;

uniform int lut_size;

layout (location = 1) out vec4 fragColor;

void main()
{
    fragColor = vec4(1);
    // // Task 17: Set fragColor using the sampler2D at the UV coordinate
    vec4 color = texture(u_texture, frag_uv);

    // Task 33: Invert fragColor's r, g, and b color channels if your bool is true
    if(bool_texture){

        // const float slices =332.0;
        // const float xOffset = 1.0 / slices;
        // const float maxSlice = slices - 1.0;
        // const float npWidth = 1.0 / (slices * slices);
        // const float npHeight = 1.0 / slices;
        // const float npHalfWidth = npWidth * 0.5;
        // const float npHalfHeight = npHeight * 0.5;

        // float x = (color.r * (xOffset - (npWidth))) + npHalfWidth;
        // float y = (color.g * (1.0 - npHeight)) + npHalfHeight;

        // float slice = color.b * maxSlice;
        // float SB = floor(slice);
        // float ST = ceil(slice);
        // float SM = fract(slice);

        // vec3 colB = texture(lut, vec2((xOffset * SB) + x, y)).rgb;
        // vec3 colT = texture(lut, vec2((xOffset * ST) + x, y)).rgb;
        // vec3 colF = mix(colB, colT, SM);
        // color = vec4(colF, 1.0);


        // Get the image color
      // Compute the 3D LUT lookup scale/offset factor
      vec3 scale = vec3((33.0 - 1.0) / 33.0);
      vec3 offset = vec3( 1.0 / (2.0 * 33.0));

      color = texture(lut, scale * color.rgb + offset);


    } else{

        color.rgb = color.rgb;
    }



    fragColor = color;
}

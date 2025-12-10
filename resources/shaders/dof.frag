#version 330 core
out vec4 fragColor;

in vec2 screen_space_uv;

uniform sampler2D normal;
uniform sampler2D light_final_col;
uniform sampler2D depth;

uniform float zfar_plane;
uniform float znear_plane;
uniform float plane_in_focus;
uniform float aperature;
uniform float focal_length;

uniform vec2 screen_space_resolution;
uniform vec4 world_cam_pos;
uniform mat4 proj_matrix;
uniform mat4 m_view;

float coc(float object_distance) {
   float coc = abs(aperature * (focal_length * (object_distance - plane_in_focus)) / (object_distance * (plane_in_focus - focal_length)));
   return coc;
}

void main() {
   float d = texture(depth, screen_space_uv).r * 2.0f - 1.0;
   float object_distance = -zfar_plane * znear_plane / (d * (zfar_plane - znear_plane) - zfar_plane);
   float c = coc(object_distance);
   float mip = clamp(c, 0.0, 7.0);
   vec4 final_color = textureLod(light_final_col, screen_space_uv, mip);
   float epsilon = 0.1;
   if (abs(object_distance - plane_in_focus) < epsilon) {
      fragColor = final_color + vec4(0.8, 0.0, 0.0, 1.0f);
   } else {
      fragColor = final_color; /*final_color;*/ /*textureLod(light_final_col, screen_space_uv, 0);*/ /*texture(normal, screen_space_uv);*/  /*vec4(d);*/ /*final_color*/
   }
   // fragColor = textureLod(light_final_col, screen_space_uv, 0);
}

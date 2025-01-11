#version 330

uniform sampler2D u_screen_tex; 
uniform int u_kernel_types;

const int w_width = 800;
const int w_height = 600;

in vec2 uv;     
out vec4 fragColor; 

float offset_x = 1.0f / w_width;  
float offset_y = 1.0f / w_height;

vec2 offsets[9] = vec2[](
    vec2(-offset_x, offset_y), vec2(0.0f, offset_y), vec2(offset_x, offset_y),
    vec2(-offset_x, 0.0f),     vec2(0.0f, 0.0f),     vec2(offset_x, 0.0f),
    vec2(-offset_x, -offset_y), vec2(0.0f, -offset_y), vec2(offset_x, -offset_y)
);

float kernel_1[9] = float[](
    1,  1,  1,
    1, -8,  1,
    1,  1,  1
);

float kernel_2[9] = float[](
    0, -1,  0,
   -1,  5, -1,
    0, -1,  0
);

float kernel_3[9] = float[](
   -2, -1,  0,
   -1,  1,  1,
    0,  1,  2
);

void main() {
    vec3 color = vec3(0.0f);

    for (int i = 0; i < 9; i++) {
        if (u_kernel_types == 0) {
            color += vec3(texture(u_screen_tex, uv + offsets[i])) * kernel_1[i];

        } else if (u_kernel_types == 1) {
            color += vec3(texture(u_screen_tex, uv + offsets[i])) * kernel_2[i];

        } else if (u_kernel_types == 2) {
            color += vec3(texture(u_screen_tex, uv + offsets[i])) * kernel_3[i];
        }
    }

    fragColor = vec4(color, 1.0f);
}

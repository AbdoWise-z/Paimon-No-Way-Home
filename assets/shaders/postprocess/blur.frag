#version 330

uniform sampler2D tex_0;
uniform sampler2D tex_1;

in vec2 tex_coord;

uniform int horizontal;

#define SIZE 10
uniform float weight[SIZE] = float[] (0.214042, 0.193062, 0.157070, 0.157070, 0.115262, 0.076292, 0.045547, 0.024526, 0.011912, 0.005218);

layout (location = 0) out vec4 out_color_0;
layout (location = 1) out vec4 out_color_1;

void main() {

    vec2 tex_offset = 1.0 / textureSize(tex_1, 0); // gets size of single texel
    vec3 result = texture(tex_1, tex_coord).rgb * weight[0]; // current fragment's contribution
    if(horizontal == 1)
    {
        for(int i = 1; i < SIZE; ++i)
        {
            result += texture(tex_1, tex_coord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(tex_1, tex_coord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < SIZE; ++i)
        {
            result += texture(tex_1, tex_coord + vec2(0.0 , tex_offset.y * i)).rgb * weight[i];
            result += texture(tex_1, tex_coord - vec2(0.0 , tex_offset.y * i)).rgb * weight[i];
        }
    }


    out_color_0 = texture(tex_0, tex_coord);
    out_color_1 = vec4(result , 1.0);
}

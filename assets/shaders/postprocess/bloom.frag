#version 330

uniform sampler2D tex_0;
uniform sampler2D tex_1;

in vec2 tex_coord;
out vec4 frag_color;

uniform float exposure; //not used for now ..
uniform float gamma = 2.2;

void main(){
    vec3 hdrColor = texture(tex_0, tex_coord).rgb;
    vec3 bloomColor = texture(tex_1, tex_coord).rgb;

    hdrColor += bloomColor; // additive blending
    frag_color = vec4(hdrColor, 1.0);
}
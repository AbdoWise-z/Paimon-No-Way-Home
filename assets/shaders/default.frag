#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normals;
} fs_in;

out vec4 frag_color;

//coloring
uniform vec4 tint;
uniform int hasTexture = 0; //1 == true , 0 == false
uniform sampler2D tex;

//lighting
uniform int spotLightsCount = 0;

//Dr forgot to add this so I added it ..
uniform float alphaThreshold;

void main(){
    //TODO: (Req 7) Modify the following line to compute the fragment color
    // by multiplying the tint with the vertex color and with the texture color
    frag_color = tint * fs_in.color * texture(tex, fs_in.tex_coord);
    if (frag_color.a < alphaThreshold){
        discard;
    }
}
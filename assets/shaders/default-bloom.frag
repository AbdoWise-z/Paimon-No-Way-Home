#version 330 core
#define MAX_LIGHTS 20

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 position;
} fs_in;

out vec4 frag_color;

//material
uniform struct Material {
    vec4 tint;
    float emission;
    int hasTexture; //1 == true , 0 == false
    sampler2D tex;
    vec3 ambientReflectivity;
    vec3 diffuseReflectivity;
    vec3 specularReflectivity;
    float specularIntensity;
} material;

void main(){
    vec4 baseColor = material.tint * fs_in.color;
    if (material.hasTexture == 1){
        baseColor = baseColor * texture(material.tex, fs_in.tex_coord);
    }

    frag_color = vec4(baseColor.rgb * vec3(material.emission), 1.0);
}
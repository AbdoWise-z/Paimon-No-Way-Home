#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normals;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 position;
} vs_out;

uniform mat4 Camera;
uniform mat4 transform;

void main(){
    gl_Position = transform * vec4(position, 1.0);
    vs_out.position = gl_Position.xyz;

    gl_Position = Camera * gl_Position;

    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = (transpose(inverse(transform)) * vec4(normals , 0.0)).xyz;
}
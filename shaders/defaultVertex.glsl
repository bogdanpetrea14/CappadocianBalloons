#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texcoord;

void main()
{
    gl_Position = projection * view * model * vec4(v_position, 1.0);
    texcoord = v_texcoord;
}

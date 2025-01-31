#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightmap;

out vec2 texcoord;

const float Y_OFFSET = 25;

void main()
{
    vec3 frag_position = v_position;
    float height = texture(heightmap, v_texcoord).r;
    frag_position.y += Y_OFFSET * height;
    gl_Position = projection * view * model * vec4(frag_position, 1.0);
    texcoord = v_texcoord;
}

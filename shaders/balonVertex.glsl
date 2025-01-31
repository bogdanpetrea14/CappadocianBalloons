#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texcoord;

void main()
{
    vec3 frag_position = v_position;
    texcoord = v_texcoord;

    if (v_position.y < 0.0) {
        float scale = 1.0 + (v_position.y / -0.5);
        frag_position.y *= scale;
    }

    gl_Position = projection * view * model * vec4(frag_position, 1.0);
}

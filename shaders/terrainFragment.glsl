#version 330
 
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D heightmap;
uniform bool mix_textures;
 
in vec2 texcoord;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 colour1 = texture2D(texture1, texcoord);
    vec4 colour2 = texture2D(texture2, texcoord);

    float height = texture(heightmap, texcoord).r;

    float factor = clamp((height - 0.2) / (0.6 - 0.2), 0.0, 1.0);

    if (mix_textures)
    {
        out_color = mix(colour1, colour2, factor);
    }
    else
    {
        out_color = colour1;
    }
}

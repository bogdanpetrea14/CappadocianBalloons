#version 330
 
uniform sampler2D texture;
 
in vec2 texcoord;

out vec4 out_color;

void main()
{
	vec4 colour1 = texture2D(texture, texcoord);
	
	out_color = colour1;
}
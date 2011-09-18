#version 150

in vec2 tex_coord;
uniform sampler2D texture0;

out vec4 frag_color;

void main(void)
{
	float color = texture(texture0, tex_coord).x;
	frag_color = vec4(color, color, color, 1.0);
}
#version 150

in vec2 position;

out vec2 tex_coord;

void main(void)
{
	gl_Position = vec4(position, 0.5, 1.0);
	tex_coord = position * 0.5 + vec2(0.5, 0.5);
}
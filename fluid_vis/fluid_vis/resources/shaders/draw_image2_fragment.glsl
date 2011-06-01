#version 150

in vec2 tex_coord;

uniform sampler2D inputImage;

out vec4 frag_color;


void main(void)
{
	frag_color = texture(inputImage, tex_coord);
	//frag_color = vec4(1.0, 0.0, 0.0, 1.0);
}
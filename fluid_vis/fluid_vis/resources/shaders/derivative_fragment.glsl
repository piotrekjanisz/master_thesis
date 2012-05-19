#version 330
precision highp float;

uniform sampler2D input_image;

in vec2 tex_coord;

out float dx;
out float dy;

void main(void)
{
	float val = texture(input_image, tex_coord).x;

	dx = dFdx(val) * 640;
	dy = dFdy(val) * 480;
}
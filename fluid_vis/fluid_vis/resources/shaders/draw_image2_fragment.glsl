#version 150

in vec2 tex_coord;

uniform sampler2D inputImage;

out vec4 frag_color;


void main(void)
{
	float color = 0.0;

	if (texture(inputImage, tex_coord).x > 0.99)
		discard;

	float stepx = 1.0 / 640.0;
	float stepy = 1.0 / 480.0;

	int filterSize = 3;

	for (int i = 0; i < filterSize; i++) {
		color += texture(inputImage, tex_coord + (i - filterSize/2)*vec2(stepx, 0.0)).x;
	}

	for (int i = 0; i < filterSize; i++) {
		color += texture(inputImage, tex_coord + (i - filterSize/2)*vec2(0.0, stepy)).x;
	}

	color /= (2*filterSize);

	frag_color = vec4(color, color, color, 1.0);
}
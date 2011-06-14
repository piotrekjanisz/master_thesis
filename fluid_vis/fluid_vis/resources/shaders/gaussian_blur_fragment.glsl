#version 150

in vec2 tex_coord;

uniform sampler2D inputImage;
uniform sampler1D gaussianDist;
uniform vec2 coordStep;

out vec4 frag_color;

void main(void)
{		
	int filter_size = textureSize(gaussianDist, 0);
	int half_filter_size = filter_size / 2;

	for (int i = 0; i < filter_size; i++) {
		vec2 coord = tex_coord + coordStep * (i - half_filter_size);
		coord = clamp(coord, vec2(0.0, 0.0), vec2(1.0, 1.0));
		vec4 sampleVal = texture(inputImage, coord);
		float g = texelFetch(gaussianDist, i, 0).x;
		frag_color += g * sampleVal;
	}
}
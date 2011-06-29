#version 150

in vec2 tex_coord;

uniform sampler2D inputImage;
uniform sampler1D gaussianDist;
uniform sampler1D spatialDist;
uniform vec2 coordStep;

out vec4 frag_color;

void main(void)
{
	float depth = texture(inputImage, tex_coord).x;
	
	if (depth > 0.99)
		discard;
		
	int filter_size = textureSize(gaussianDist, 0);
	int half_filter_size = filter_size / 2;

	float sum = 0.0;
	float wsum = 0.0;

	for (int i = 0; i < filter_size; i++) {
		vec2 coord = tex_coord + coordStep * (i - half_filter_size);
		coord = clamp(coord, vec2(0.0, 0.0), vec2(1.0, 1.0));
		float sampleVal = texture(inputImage, coord).x;
		float w = texelFetch(gaussianDist, i, 0).x;
		float g = texture(spatialDist, abs(sampleVal - depth)).x;
		w *= g;
		sum += w * sampleVal;
		wsum += w;
	}

	if (wsum > 0.0)
		sum /= wsum;		

	frag_color = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragDepth = sum;
}
#version 150

in vec2 tex_coord;

uniform sampler2D linearDepth;
uniform sampler1DArray gaussianDist;
uniform sampler1D spatialDist;

uniform float farDist;
uniform int maxFilterSize;
uniform int maxPointSize;
uniform int maxFilter;
uniform vec2 coordStep;

uniform float filterSizeMult;

out float frag_color;

void main(void)
{
	// float depth = texture(inputImage, tex_coord).x;
	
	float center_lin_depth = texture(linearDepth, tex_coord).x;

		
	if (center_lin_depth > 0.99)
		discard;

	float pointSize = maxPointSize / (center_lin_depth * farDist);	// good approximation when near close to 0.0;
	int filterIndex = clamp(int(floor((pointSize * filterSizeMult) / 2.0)), 0, maxFilter);
	int filter_size = 2 * filterIndex + 1;
	int half_filter_size = filter_size / 2;

	float sum = 0.0;
	float wsum = 0.0;

	for (int i = 0; i < filter_size; i++) {
		vec2 coord = tex_coord + coordStep * (i - half_filter_size);
		coord = clamp(coord, vec2(0.0, 0.0), vec2(1.0, 1.0));
		float lin_depth = texture(linearDepth, coord).x;
		float w = texelFetch(gaussianDist, ivec2(i, filterIndex), 0).x;
		float g = texture(spatialDist, abs(lin_depth - center_lin_depth)).x;

		w *= g;
		sum += w * lin_depth;
		wsum += w;
	}

	if (wsum > 0.0)
		sum /= wsum;		
	else 
		sum = 1.0;
	if (sum <= 0.0)
		sum = 1.0; // FIXME 0.0 maybe better

	frag_color = sum;
}
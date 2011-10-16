#version 150

in vec2 tex_coord;

uniform sampler2D inputImage;
uniform vec2 coordStep;
uniform float treshold;

out float frag_color;


void main()
{
	float sampleValue = texture(inputImage, tex_coord).x;

	float upValue = texture(inputImage, tex_coord + vec2(0.0, -coordStep.y)).x;
	float downValue = texture(inputImage, tex_coord + vec2(0.0, coordStep.y)).x;
	float rightValue = texture(inputImage, tex_coord + vec2(coordStep.x, 0.0)).x;
	float leftValue = texture(inputImage, tex_coord + vec2(-coordStep.x, 0.0)).x;

	frag_color = abs(sampleValue - upValue) > treshold ? 0.0 : 1.0;
	frag_color = abs(sampleValue - downValue) > treshold ? 0.0 : 1.0;
	frag_color = abs(sampleValue - rightValue) > treshold ? 0.0 : 1.0;
	frag_color = abs(sampleValue - leftValue) > treshold ? 0.0 : 1.0;
}
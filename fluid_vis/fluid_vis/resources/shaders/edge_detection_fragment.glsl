#version 150

in vec2 tex_coord;

uniform sampler2D inputImage;
uniform vec2 coordStep;
uniform float treshold;

out float frag_color;


void main()
{
	float sampleValue = texture(inputImage, tex_coord).x;

	float upValue = texture(inputImage, tex_coord + vec2(0.0, coordStep.y)).x;
	float downValue = texture(inputImage, tex_coord + vec2(0.0, -coordStep.y)).x;
	float rightValue = texture(inputImage, tex_coord + vec2(coordStep.x, 0.0)).x;
	float leftValue = texture(inputImage, tex_coord + vec2(-coordStep.x, 0.0)).x;

	float upRightValue = texture(inputImage, tex_coord + vec2(coordStep.x, coordStep.y)).x;
	float upLeftValue = texture(inputImage, tex_coord + vec2(-coordStep.x, coordStep.y)).x;
	float downRightValue = texture(inputImage, tex_coord + vec2(coordStep.x, -coordStep.y)).x;
	float downLeftValue = texture(inputImage, tex_coord + vec2(-coordStep.x, -coordStep.y)).x;

	float max_diff = 0.0;
	max_diff = max(max_diff, abs(sampleValue - upValue));
	max_diff = max(max_diff, abs(sampleValue - downValue));
	max_diff = max(max_diff, abs(sampleValue - rightValue));
	max_diff = max(max_diff, abs(sampleValue - leftValue));
	max_diff = max(max_diff, abs(sampleValue - upRightValue));
	max_diff = max(max_diff, abs(sampleValue - upLeftValue));
	max_diff = max(max_diff, abs(sampleValue - downRightValue));
	max_diff = max(max_diff, abs(sampleValue - downLeftValue));

	frag_color = max_diff > treshold ? 0.0 : 1.0;
}
#version 150

in vec2 tex_coord;

uniform sampler2D depthTexture;
uniform sampler2D sceneDepthTexture;
uniform sampler2D sceneTexture;
uniform vec2 coordStep;

out vec4 frag_color;

void main(void)
{
	float depth = texture(depthTexture, tex_coord).x;
	float sceneDepth = texture(sceneDepthTexture, tex_coord).x;
	
	if (depth < sceneDepth) {
		frag_color = vec4(depth, depth, depth, 1.0);
	} else {
		frag_color = texture(sceneTexture, tex_coord);
	}

	/*
	float color = texture(depthTexture, tex_coord).x;
	frag_color = vec4(color, color, color, 1.0);
	*/
}
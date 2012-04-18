#version 150

in vec2 tex_coord;

uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

out vec4 frag_color;

void main(void)
{		
	gl_FragDepth = texture(depthTexture, tex_coord).x;
	frag_color = texture(colorTexture, tex_coord);
}
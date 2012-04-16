#version 150

in vec2 tex_coord;

uniform sampler2D texture1;

out vec4 frag_color;

void main(void)
{		
	float depth = texture(texture1, tex_coord).x;

	gl_FragDepth =  depth;

	frag_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
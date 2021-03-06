#version 150

uniform samplerCube cubeMap;

in vec3 tex_coord;

out vec4 frag_color;

void main(void)
{
	frag_color = texture(cubeMap, tex_coord);
}
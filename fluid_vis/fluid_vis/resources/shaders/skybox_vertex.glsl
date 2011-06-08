#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec4 vertex;

out vec3 tex_coord;

void main(void)
{
	tex_coord = normalize(vertex.xyz);
	gl_Position = projectionMatrix * modelViewMatrix * vertex;
}
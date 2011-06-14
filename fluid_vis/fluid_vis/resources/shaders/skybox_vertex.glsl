#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

in vec4 vertex;

out vec3 tex_coord;

void main(void)
{
	tex_coord = normalize(vertex.xyz);
	gl_Position = projectionMatrix * mat4(modelViewMatrix[0], modelViewMatrix[1], modelViewMatrix[2], vec4(0.0, 0.0, 0.0, 1.0)) * vertex;
}
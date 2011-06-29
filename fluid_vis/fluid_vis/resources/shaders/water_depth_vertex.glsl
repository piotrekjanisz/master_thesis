#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform float pointSize;
in vec4 vertex;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;

	float len = length(v);

	gl_PointSize = pointSize / len;

	gl_Position = projectionMatrix * v;
}
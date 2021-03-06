#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform float pointSize;
in vec4 vertex;

flat out vec3 eyeSpacePos;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;
	eyeSpacePos = v.xyz;

	float len = length(v);

	gl_PointSize = pointSize / len;

	gl_Position = projectionMatrix * v;
}
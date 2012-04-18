#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform float far;
in vec4 vertex;
in vec3 normal;

out vec3 fNormal;
out vec3 fEye;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;
	fEye = -normalize(v.xyz);
	fNormal = normalMatrix * normalize(normal);
	gl_Position = projectionMatrix * v;
}
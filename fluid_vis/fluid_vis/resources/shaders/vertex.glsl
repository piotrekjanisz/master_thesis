#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
in vec4 vertex;
in vec3 normal;
in vec2 tex_coord;

out vec3 fNormal;
out vec2 fragment_tex_coord;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;
	fNormal = normalMatrix * normal;
	gl_Position = projectionMatrix * v;
	fragment_tex_coord = tex_coord;
}
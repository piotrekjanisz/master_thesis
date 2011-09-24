#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform vec3 lightDirection;

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 tex_coord;

out vec2 fragment_tex_coord;
out vec3 fLightDir;

void main(void)
{
	gl_Position = modelViewProjectionMatrix * vertex;
	fragment_tex_coord = tex_coord;

	mat3 tbnMatrix = mat3(tangent, bitangent, normal);
	fLightDir = tbnMatrix * lightDirection;
}
#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform vec4 lightPositionEyeSpace;

in vec4 vertex;
in vec3 normal;
in vec2 tex_coord;
in vec3 tangent;
in vec3 bitangent;

out Vertex
{
	out vec3 lightVec;
	out vec3 normal;
	out vec3 tangent;
	out vec3 bitangent;
} vert;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;
	gl_Position = v;

	vec3 eVec = -normalize(vec3(v));
	vec3 lVec = normalize(vec3(lightPositionEyeSpace - v));

	vert.lightVec = lVec;
	vert.normal = normalize(normalMatrix * normal);
	vert.tangent = normalize(normalMatrix * tangent);
	vert.bitangent = normalize(normalMatrix * bitangent);
}
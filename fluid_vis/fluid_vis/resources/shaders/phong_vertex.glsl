#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform vec4 lightPositionEyeSpace;

in vec4 vertex;
in vec3 normal;
in vec2 tex_coord;

out vec2 fragment_tex_coord;
out vec3 lightVec;
out vec3 halfVec;
out vec3 eyeVec;
out vec3 eyeNormal;


void main(void)
{
	vec4 v = modelViewMatrix * vertex;
	gl_Position = projectionMatrix * v;

	vec3 eVec = -normalize(vec3(v));
	vec3 lVec = normalize(vec3(lightPositionEyeSpace - v));

	eyeVec = eVec;
	lightVec = lVec;
	halfVec = normalize(eVec + lVec);
	eyeNormal = normalize(normalMatrix * normal);
	fragment_tex_coord = tex_coord;
}
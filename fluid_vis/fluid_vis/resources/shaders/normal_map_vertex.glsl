#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
//uniform mat4 modelViewProjectionMatrix;
uniform vec4 lightPositionEyeSpace;

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 tex_coord;

out vec2 fragment_tex_coord;
out vec3 lightVec;
out vec3 halfVec;
out vec3 eyeVec;

void main(void)
{
	vec4 vertexPosition = modelViewMatrix * vertex;	
	gl_Position = projectionMatrix * vertexPosition;

	fragment_tex_coord = tex_coord;

	vec3 n = normalize(normalMatrix * normal);
	vec3 t = normalize(normalMatrix * tangent);
	vec3 b = normalize(normalMatrix * bitangent);

	vec3 lightDir = normalize(vec3(lightPositionEyeSpace - vertexPosition));

	vec3 v;
	v.x = dot(lightDir, t);
	v.y = dot(lightDir, b);
	v.z = dot(lightDir, n);
	lightVec = normalize (v);

	v.x = dot(vertexPosition.xyz, t);
	v.y = dot(vertexPosition.xyz, b);
	v.z = dot(vertexPosition.xyz, n);
	eyeVec = normalize (v);

	vec3 halfVector = -normalize(vertexPosition.xyz) + lightDir;
	v.x = dot(halfVector, t);
	v.y = dot(halfVector, b);
	v.z = dot(halfVector, n);

	// No need to normalize, t,b,n and halfVector are normal vectors.
	//normalize (v);
	halfVec = normalize(v);
}
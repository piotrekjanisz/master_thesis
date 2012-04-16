#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform float pointSize;
uniform float minDensity;
uniform float normalDensity;
uniform float maxPointSize;
uniform int particleDepthExp;

in vec4 vertex;
in float density;

flat out vec3 eyeSpacePos;

const float MIN_DENSITY = 300.0;
const float NORMAL_DENSITY = 800.0;

const int MAX_POINTSIZE = 120;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;
	eyeSpacePos = v.xyz;

	if (density < MIN_DENSITY)
		gl_PointSize = 0;
	else
		//gl_PointSize = min(pow((density / NORMAL_DENSITY), 2) * pointSize, MAX_POINTSIZE) / abs(eyeSpacePos.z);
		gl_PointSize = min(pow((density / normalDensity), particleDepthExp) * pointSize, maxPointSize) / abs(v.z);

	gl_Position = projectionMatrix * v;
}
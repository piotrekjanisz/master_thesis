#version 150

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

uniform float pointSize;
uniform float minDensity;
uniform float normalDensity;
uniform float maxPointSize;
uniform float particleDepth;
uniform int particleDepthExp;

in vec4 vertex;
in float density;

out float fragParticleDepth;

const float MIN_DENSITY = 300.0;
const float NORMAL_DENSITY = 800.0;
const int MAX_POINTSIZE = 120;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;

	if (density < MIN_DENSITY) {
		gl_PointSize = 0;
		fragParticleDepth = 0.0f;
	} else {
		//gl_PointSize = min(pow((density / NORMAL_DENSITY), 2) * pointSize, MAX_POINTSIZE) / abs(v.z);
		gl_PointSize = min(pow((density / normalDensity), particleDepthExp) * pointSize, maxPointSize) / abs(v.z);
		fragParticleDepth = min(pow(density / normalDensity, particleDepthExp) * particleDepth, 0.5f);
	}

	gl_Position = projectionMatrix * v;
}
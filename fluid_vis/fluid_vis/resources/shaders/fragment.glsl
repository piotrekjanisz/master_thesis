#version 150

uniform vec4 color;

in vec3 fNormal;
out vec4 fragColor;

void main(void)
{
	float diffuseIntensity = max(0.0f, dot(fNormal, vec3(1.0f, 1.0f, 1.0f)));
	//fragColor = vec4(0.1f, 0.1f, 0.1f, 1.0f) + vec4(1.0f, 0.0f, 0.0f, 1.0f) * diffuseIntensity;
	fragColor = color * 0.2 + color * diffuseIntensity;
}

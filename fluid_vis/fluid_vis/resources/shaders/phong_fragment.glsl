#version 150

uniform sampler2D tex;

in vec2 fragment_tex_coord;
in vec3 lightVec;
in vec3 halfVec;
in vec3 eyeVec;
in vec3 eyeNormal;

out vec4 fragColor;

const float AMBIENT_INTENSITY = 0.2;

void main(void)
{
	float diffuseIntensity = max(dot(lightVec, eyeNormal), 0.0);
	float specularIntensity = pow(max(dot(halfVec, eyeNormal), 0.0), 16);

	//specularIntensity = 0.0;

	vec4 textureColor = texture(tex, fragment_tex_coord);

	fragColor = 0.2 * textureColor + diffuseIntensity * textureColor + specularIntensity * vec4(1.0, 1.0, 1.0, 1.0);
}
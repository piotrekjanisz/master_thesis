#version 150

uniform vec4 color;
uniform sampler2D tex;
uniform vec4 lightDirection;

in vec3 fNormal;
in vec2 fragment_tex_coord;
out vec4 fragColor;

void main(void)
{
	vec3 reflection = normalize(reflect(lightDirection.xyz, fNormal));
	float diffuseIntensity = max(0.0f, dot(fNormal, reflection));
	//fragColor = vec4(0.1f, 0.1f, 0.1f, 1.0f) + vec4(1.0f, 0.0f, 0.0f, 1.0f) * diffuseIntensity;
	fragColor = texture(tex, fragment_tex_coord) * 0.2 + texture(tex, fragment_tex_coord) * diffuseIntensity;
}

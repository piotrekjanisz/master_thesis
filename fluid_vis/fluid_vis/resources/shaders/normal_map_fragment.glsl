#version 150

uniform sampler2D tex;
uniform sampler2D normal_map;

in vec3 fLightDir;
in vec2 fragment_tex_coord;
out vec4 fragColor;

const float AMBIENT_INTENSITY = 0.2;

void main(void)
{
	vec3 fNormal = 2*(texture(normal_map, fragment_tex_coord).rgb - vec3(0.5, 0.5, 0.5));
	vec3 reflection = normalize(reflect(fLightDir, fNormal));
	float diffuseIntensity = max(0.0f, dot(fNormal, reflection));
	fragColor = texture(tex, fragment_tex_coord) * AMBIENT_INTENSITY + texture(tex, fragment_tex_coord) * diffuseIntensity;
}

#version 150

uniform sampler2D tex;
uniform sampler2D normal_map;


in vec3 lightVec;
in vec3 halfVec;
in vec3 eyeVec;
in vec2 fragment_tex_coord;	
in vec3 vNormal;
out vec4 fragColor;

const float AMBIENT_INTENSITY = 0.2;

void main(void)
{
	vec3 normal = normalize(2.0 * texture(normal_map, fragment_tex_coord).rgb - 1.0);
	
	float diffuseIntensity = clamp(dot(lightVec, normal), 0.0, 1.0) ;
	float specularIntensity = pow(max(dot(normalize(halfVec), normal), 0.0), 16) ;

	vec4 textureColor = texture(tex, fragment_tex_coord);
	
	fragColor = 0.2 * textureColor + diffuseIntensity * textureColor + specularIntensity * vec4(0.5, 0.5, 0.5, 0.0);
}

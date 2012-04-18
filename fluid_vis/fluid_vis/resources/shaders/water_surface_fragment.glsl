#version 150

in vec3 fNormal;
in vec3 fEye;
out vec4 fragColor;
uniform sampler2D sceneTexture;
uniform vec2 screenSize;

void main(void)
{
	float diffuseIntensity;
	float specularItensity;

	vec3 light;

	vec3 normal;
	vec3 eye;
	
	vec3 reflection;

	vec2 tex_coord = gl_FragCoord.xy / screenSize;

	light = normalize(vec3(5.0, 5.0, 5.0));

	normal = fNormal;
	eye = normalize(fEye);

	diffuseIntensity = clamp(max(dot(normal, light), 0.0), 0.0, 1.0);
	
	reflection = normalize(reflect(-light, normal));
	specularItensity = pow(clamp(max(dot(reflection, eye), 0.0), 0.0, 1.0), 5.0 );

	vec4 water_color = vec4(0.0, 10.0/255.0, 79.0/255.0, 1.0);
	fragColor = mix(water_color, texture(sceneTexture, 0.1 * normal.xy + tex_coord), 0.9);
	fragColor +=  vec4(1.0, 0.9, 0.9, 1.0)*specularItensity;

	//fragColor = vec4(0.0, 0.0, 0.0, 1.0) + vec4(0.1, 0.1, 0.1, 1.0) + vec4(1.0, 0.0, 0.0, 1.0)*diffuseIntensity + vec4(1.0, 0.9, 0.9, 1.0)*specularItensity;
}

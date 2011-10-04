#version 150

in vec3 fNormal;
in vec3 fEye;
out vec4 fragColor;

void main(void)
{/*
	float diffuseIntensity = max(0.0f, dot(fNormal, normalize(vec3(1.0f, 1.0f, 1.0f))));
	vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
	//fragColor = vec4(0.1f, 0.1f, 0.1f, 1.0f) + vec4(1.0f, 0.0f, 0.0f, 1.0f) * diffuseIntensity;
	fragColor = color * diffuseIntensity;
*/
	float diffuseIntensity;
	float specularItensity;

	vec3 light;

	vec3 normal;
	vec3 eye;
	
	vec3 reflection;

	light = normalize(vec3(5.0, 5.0, 5.0));

	normal = fNormal;
	eye = normalize(fEye);

	diffuseIntensity = clamp(max(dot(normal, light), 0.0), 0.0, 1.0);
	
	reflection = normalize(reflect(-light, normal));
	specularItensity = pow(clamp(max(dot(reflection, eye), 0.0), 0.0, 1.0), 5.0 );

	fragColor = vec4(0.0, 0.0, 0.0, 1.0) + vec4(0.1, 0.1, 0.1, 1.0) + vec4(1.0, 0.0, 0.0, 1.0)*diffuseIntensity + vec4(1.0, 0.9, 0.9, 1.0)*specularItensity;
}

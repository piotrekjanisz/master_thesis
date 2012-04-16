#version 150

in vec2 tex_coord;

uniform sampler2D wTexture;
uniform sampler2D depthTexture;
uniform sampler2D waterDepthTexture;
uniform sampler2D sceneDepthTexture;
uniform sampler2D sceneTexture;
uniform samplerCube cubeMap;

uniform vec2 coordStep;
uniform vec4 lightDirection;

uniform float ctg_fov_x;
uniform float ctg_fov_y;
uniform float far;
uniform float refractionMult;

out vec4 frag_color;

vec3 unproject(vec2 xy, float depth)
{
	vec3 v_eye;
	v_eye.z = depth * far;
	v_eye.x = v_eye.z * (2 * xy.x - 1) / ctg_fov_x;
	v_eye.y = v_eye.z * (2 * xy.y - 1) / ctg_fov_y;
	v_eye.z = -v_eye.z;

	return v_eye;
}

void main(void)
{
	float depth = texture(depthTexture, tex_coord).x;
	float sceneDepth = texture(sceneDepthTexture, tex_coord).x;
	
	if (depth < sceneDepth) {
		vec3 v_eye = unproject(tex_coord, texture(wTexture, tex_coord).x);

		vec2 tex_coord2;
		tex_coord2 = tex_coord + vec2(coordStep.x, 0.0);
		vec3 ddx = unproject(tex_coord2, texture(wTexture, tex_coord2).x) - v_eye;
		tex_coord2 = tex_coord - vec2(coordStep.x, 0.0);
		vec3 ddx2 = v_eye - unproject(tex_coord2, texture(wTexture, tex_coord2).x);
		if (abs(ddx.z) > abs(ddx2.z))
			ddx = ddx2;


		tex_coord2 = tex_coord + vec2(0.0, coordStep.y);
		vec3 ddy = unproject(tex_coord2, texture(wTexture, tex_coord2).x) - v_eye;
		tex_coord2 = tex_coord - vec2(0.0, coordStep.y);
		vec3 ddy2 = v_eye - unproject(tex_coord2, texture(wTexture, tex_coord2).x);

		if (abs(ddy.z) > abs(ddy2.z))
			ddy = ddy2;
		
		vec3 normal = normalize(cross(ddx, ddy));

		vec3 eye = vec3 (0.0, 0.0, 1.0);

		//----------------NEW CODE--------------
		vec4 water_color = vec4(0.0, 10.0/255.0, 79.0/255.0, 1.0);

		float waterDepth = texture(waterDepthTexture, tex_coord).x;
		vec4 refracted_color = mix(water_color, texture(sceneTexture, tex_coord + refractionMult * waterDepth * vec2(normal.x, normal.y)), exp(-5 * waterDepth));

		vec3 reflection_vector = normalize(reflect(normal, eye));
		//vec4 reflected_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vec4 reflected_color = texture(cubeMap, reflection_vector);

		float r_0 = 0.1;
		float fresnel = r_0 + (1.0 - r_0) * pow(1 - dot(eye, normal), 5);

		vec3 halfVector = normalize(eye - normalize(lightDirection.xyz));
		float fSpec = pow(max(dot(normal, halfVector), 0), 64);
		//float fSpec = 0.0f;

		frag_color = (1.0 - fresnel) * refracted_color + fresnel * reflected_color + vec4(fSpec, fSpec, fSpec, 1.0f);
		//----------------NEW CODE--------------

		//vec3 halfVector = normalize(eye - normalize(lightDirection.xyz));
//
		//float fSpec = pow(max(dot(normal, halfVector), 0), 64);
//
		//float waterDepth = texture(waterDepthTexture, tex_coord).x;
		//
		//frag_color = mix(vec4(0.0, 10.0/255.0, 79.0/255.0, 1.0), texture(sceneTexture, tex_coord + refractionMult * waterDepth * vec2(normal.x, normal.y)), exp(-2 * waterDepth));
		//
		//frag_color.rgb += vec3(fSpec, fSpec, fSpec);
	} else {
		frag_color = texture(sceneTexture, tex_coord);
	}
}
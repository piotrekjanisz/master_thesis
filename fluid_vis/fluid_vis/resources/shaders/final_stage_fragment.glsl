#version 150

in vec2 tex_coord;

uniform sampler2D depthTexture;
uniform sampler2D sceneDepthTexture;
uniform sampler2D sceneTexture;
uniform vec2 coordStep;
uniform mat4 inverseProjection;

out vec4 frag_color;

vec3 unproject(vec2 xy, float depth, mat4 inverseProjection)
{
	vec4 v_screen = vec4(xy, depth, 1.0);
	vec4 v_homo = inverseProjection * (2.0 * (v_screen - vec4(0.5)));
	vec3 v_eye = v_homo.xyz / v_homo.w;
	return v_eye;
}

void main(void)
{
	float depth = texture(depthTexture, tex_coord).x;
	float sceneDepth = texture(sceneDepthTexture, tex_coord).x;
	
	if (depth < sceneDepth) {
		vec3 v_eye = unproject(tex_coord, depth, inverseProjection);

		vec2 tex_coord2;
		tex_coord2 = tex_coord + vec2(coordStep.x, 0.0);
		vec3 ddx = unproject(tex_coord2, texture(depthTexture, tex_coord2).x, inverseProjection) - v_eye;
		tex_coord2 = tex_coord - vec2(coordStep.x, 0.0);
		vec3 ddx2 = v_eye - unproject(tex_coord2, texture(depthTexture, tex_coord2).x, inverseProjection);
		if (abs(ddx.z) > abs(ddx2.z))
			ddx = ddx2;


		tex_coord2 = tex_coord + vec2(0.0, coordStep.y);
		vec3 ddy = unproject(tex_coord2, texture(depthTexture, tex_coord2).x, inverseProjection) - v_eye;
		tex_coord2 = tex_coord - vec2(0.0, coordStep.y);
		vec3 ddy2 = v_eye - unproject(tex_coord2, texture(depthTexture, tex_coord2).x, inverseProjection);

		if (abs(ddy.z) > abs(ddy2.z))
			ddy = ddy2;

		vec3 normal = normalize(cross(ddx, ddy));

		vec3 reflection = normalize(reflect(vec3(-1.0, -1.0, -1.0), normal));
		float spec = max(0.0, dot(normal, reflection));

		frag_color = 0.8 * texture(sceneTexture, tex_coord + 0.1 * normal.xy) + vec4(0.2, 0.0, 0.0, 1.0);
		float powSpec = pow(spec, 64.0);
		frag_color.rgb += vec3(powSpec, powSpec, powSpec);
	} else {
		frag_color = texture(sceneTexture, tex_coord);
	}
}
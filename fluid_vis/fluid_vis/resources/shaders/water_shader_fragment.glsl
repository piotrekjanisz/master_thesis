#version 150

flat in vec3 eyeSpacePos;
uniform mat4 projectionMatrix;
out vec4 fragColor;

const float coef1 = 0.5f;
const float coef2 = 0.5f;

void main(void)
{
	vec4 normal;
    normal.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0);

    float mag = dot(normal.xy, normal.xy);

    if (mag > 1.0f)
        discard;

    normal.z = sqrt(1.0f - mag);
    normal.w = 1.0f;

	vec4 pixelPos = vec4(eyeSpacePos + 0.1f * normal.xyz, 1.0f);
	vec4 clipSpacePos = projectionMatrix * pixelPos;
	// clipSpacePos.z / clipSpacePos.w is in [-1, 1] convert it to [0,1]
	float depth = (clipSpacePos.z / clipSpacePos.w)*coef1 + coef2; 
	gl_FragDepth = depth;
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
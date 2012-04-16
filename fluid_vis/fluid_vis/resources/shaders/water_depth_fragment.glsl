#version 150


in float fragParticleDepth;

out vec4 fragColor;

void main(void)
{
	vec4 normal;
    normal.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0);

    float mag = dot(normal.xy, normal.xy);

    if (mag > 1.0f)
        discard;

	float color = fragParticleDepth;

	fragColor = vec4(color, color, color, 1.0);
}
#version 150

out vec4 fragColor;

void main(void)
{
	vec4 normal;
    normal.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0, 1.0);

    float mag = dot(normal.xy, normal.xy);

    if (mag > 1.0f)
        discard;

    normal.z = sqrt(1.0f - mag);
    normal.w = 1.0f;

    float diffuseIntensity = max(dot(normal.xyz, vec3(1.0f, 1.0f, 1.0f)), 0.0);
    
    fragColor = vec4(0.1, 0.1, 0.1, 1.0) + vec4(1.0, 0.0, 0.0, 1.0)*diffuseIntensity;
}
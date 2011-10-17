#version 150

in vec2 tex_coord;

uniform sampler2D depthTexture;
uniform sampler2D edgeTexture;

uniform float cx;
uniform float cy;
uniform float time_step;

uniform vec2 coordStep;

out float frag_color;

// TODO pass far plane to shader

void main()
{
	float z = texture(depthTexture, tex_coord).x;
	float z_top = texture(depthTexture, tex_coord + vec2(0.0, coordStep.y)).x;
	float z_bot = texture(depthTexture, tex_coord + vec2(0.0, -coordStep.y)).x;
	float z_left = texture(depthTexture, tex_coord + vec2(-coordStep.x, 0.0)).x;
	float z_right = texture(depthTexture, tex_coord + vec2(coordStep.x, 0.0)).x;

	float edge = texture(edgeTexture, tex_coord).x;

	float dzx = (z_right - z) / coordStep.x;
	float dzy = (z_top - z) / coordStep.y;

	float ddzx = (z_right - 2*z + z_left) / (coordStep.x * coordStep.x);
	float ddzy = (z_top - 2*z + z_bot) / (coordStep.y * coordStep.y);

	float D = cy*cy*dzx*dzx + cx*cx*dzy*dzy + cx*cx*cy*cy*z*z;

	float dDx = dFdx(D);
	float dDy = dFdy(D);

	float Ex = 0.5 * dzx * dDx - ddzx*D;
	float Ey = 0.5 * dzy * dDy - ddzy*D;

	float H = 0.5 * (cy*Ex + cx*Ey) / pow(D, 1.5);

	frag_color = z + time_step * H;
}
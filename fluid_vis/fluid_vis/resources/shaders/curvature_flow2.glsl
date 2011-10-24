#version 150

in vec2 tex_coord;

uniform sampler2D depthTexture;
uniform sampler2D edgeTexture;

uniform float Cx;
uniform float Cy;
uniform float Cx_square;
uniform float Cy_square;
uniform float timeStep;

uniform vec2 coordStep;
uniform vec2 coordStepInv;

out float frag_color;

void main()
{
	float z = texture(depthTexture, tex_coord).x;
	float edge = texture(edgeTexture, tex_coord).x;

	float z_top = texture(depthTexture, tex_coord + vec2(0.0, coordStep.y)).x;
	float z_bot = texture(depthTexture, tex_coord + vec2(0.0, -coordStep.y)).x;
	float z_left = texture(depthTexture, tex_coord + vec2(-coordStep.x, 0.0)).x;
	float z_right = texture(depthTexture, tex_coord + vec2(coordStep.x, 0.0)).x;
	
	float dzx = edge * (z - z_left) * coordStepInv.x;
	float dzy = edge * (z - z_bot) * coordStepInv.y;

	float ddzx = edge * (z_right - 2*z + z_left) * (coordStepInv.x * coordStepInv.x);
	float ddzy = edge * (z_top - 2*z + z_bot) * (coordStepInv.y * coordStepInv.y);

	float D = Cy_square*dzx*dzx + Cx_square*dzy*dzy + Cx_square*Cy_square*z*z;

	// FIXME is dFdx appropriate?
	float dDx = dFdx(D) * coordStepInv.x;
	float dDy = dFdy(D) * coordStepInv.y;

	float Ex = 0.5 * dzx * dDx - ddzx*D;
	float Ey = 0.5 * dzy * dDy - ddzy*D;

	float H = 0.5 * (Cy*Ex + Cx*Ey) / pow(D, 1.5);

	frag_color = z + timeStep * H;
}
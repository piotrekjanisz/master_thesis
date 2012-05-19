#version 330
precision highp float;

uniform sampler2D depthTexture;
uniform sampler2D edgeTexture;
uniform sampler2D dxTexture;
uniform sampler2D dyTexture;

uniform float Cx;
uniform float Cy;
uniform float Cx_square;
uniform float Cy_square;
uniform float timeStep;

uniform vec2 coordStep;
uniform vec2 coordStepInv; // = vec2(width, height)

input vec2 tex_coord;

out float frag_color;

void main(void)
{
	float z_dep = texture(depthTexture, tex_coord).x;
	float z = z_dep * 100.0;
	float edge = texture(edgeTexture, tex_coord).x;

	float dzx = texture(dxTexture, tex_coord).x;
	float dzy = texture(dyTexture, tex_coord).x;

	float ddzx = dFdx(dzx) * coordStepInv.x;
	float ddzy = dFdy(dzy) * coordStepInv.y;

	float D = Cy_square*dzx*dzx + Cx_square*dzy*dzy + Cx_square*Cy_square*z*z;
	
	// FIXME is dFdx appropriate?
	float dDx = dFdx(D) * coordStepInv.x;
	float dDy = dFdy(D) * coordStepInv.y;

	float Ex = 0.5 * dzx * dDx - ddzx*D;
	float Ey = 0.5 * dzy * dDy - ddzy*D;

	float H = edge * 0.5 * (Cy*Ex + Cx*Ey) / (pow(D, 1.5));

	frag_color = z_dep + timeStep * H;
}
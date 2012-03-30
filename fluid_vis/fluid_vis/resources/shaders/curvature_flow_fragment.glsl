#version 330
precision highp float;

in vec2 tex_coord;

uniform sampler2D depthTexture;
uniform sampler2D edgeTexture;

uniform float Cx;
uniform float Cy;
uniform float Cx_square;
uniform float Cy_square;
uniform float timeStep;

uniform vec2 coordStep;
uniform vec2 coordStepInv; // = vec2(width, height)

out float frag_color;

void main()
{
	float z_dep = texture(depthTexture, tex_coord).x;
	float z = z_dep * 100.0;
	float edge = texture(edgeTexture, tex_coord).x;
	float edge_t = texture(edgeTexture, tex_coord + vec2(0.0, coordStep.y)).x;
	float edge_r = texture(edgeTexture, tex_coord + vec2(coordStep.x, 0.0)).x;
	float edge_b = texture(edgeTexture, tex_coord + vec2(-coordStep.x, 0.0)).x;
	float edge_l = texture(edgeTexture, tex_coord + vec2(0.0, -coordStep.y)).x;

	edge = edge * edge_t * edge_r * edge_l * edge_b;

	float z_top = texture(depthTexture, tex_coord + vec2(0.0, coordStep.y)).x * 100.0;
	float z_bot = texture(depthTexture, tex_coord + vec2(0.0, -coordStep.y)).x * 100.0;
	float z_left = texture(depthTexture, tex_coord + vec2(-coordStep.x, 0.0)).x * 100.0;
	float z_right = texture(depthTexture, tex_coord + vec2(coordStep.x, 0.0)).x * 100.0;
	/*
	float z_t = texture(depthTexture, tex_coord + vec2(0.0, coordStep.y)).x;
	float z_b = texture(depthTexture, tex_coord + vec2(0.0, -coordStep.y)).x;
	float z_l = texture(depthTexture, tex_coord + vec2(-coordStep.x, 0.0)).x;
	float z_r = texture(depthTexture, tex_coord + vec2(coordStep.x, 0.0)).x;

	float z_tl = texture(depthTexture, tex_coord + vec2(-coordStep.x, coordStep.y)).x;
	float z_tt = texture(depthTexture, tex_coord + 2*vec2(0.0, coordStep.y)).x;
	float z_tr = texture(depthTexture, tex_coord + vec2(coordStep.x, coordStep.y)).x;
	float z_rr = texture(depthTexture, tex_coord + 2*vec2(coordStep.x, 0.0)).x;
	float z_br = texture(depthTexture, tex_coord + vec2(coordStep.x, -coordStep.y)).x;
	*/

	float dzx = (z_right - z) * coordStepInv.x;
	float dzy = (z_top - z) * coordStepInv.y;
	/*
	float dzx_t = (z_tr - z_t) * coordStepInv.x;
	float dzy_t = (z_tt - z_t) * coordStepInv.y;

	float dzx_r = (z_rr - z_r) * coordStepInv.x;
	float dzy_r = (z_tr - z_r) * coordStepInv.y;
	*/
	float ddzx = (z_right - 2*z + z_left) * (coordStepInv.x * coordStepInv.x);
	float ddzy = (z_top - 2*z + z_bot) * (coordStepInv.y * coordStepInv.y);
	/*
	float ddzx_t = (z_tr - 2*z_t + z_tl) * (coordStepInv.x * coordStepInv.x);
	float ddzy_t = (z_tt - 2*z_t + z) * (coordStepInv.y * coordStepInv.y);

	float ddzx_r = (z_rr - 2*z_r + z) * (coordStepInv.x * coordStepInv.x);
	float ddzy_r = (z_tr - 2*z_r + z_br) * (coordStepInv.y * coordStepInv.y);
	*/
	float D = Cy_square*dzx*dzx + Cx_square*dzy*dzy + Cx_square*Cy_square*z*z;
	/*
	float D_t = Cy_square*dzx_t*dzx_t * Cx_square*dzy_t*dzy_t + Cx_square*Cy_square*z_t*z_t;
	float D_r = Cy_square*dzx_r*dzx_r * Cx_square*dzy_r*dzy_r + Cx_square*Cy_square*z_r*z_r;
	*/
	
	// FIXME is dFdx appropriate?
	float dDx = dFdx(D) * coordStepInv.x;
	float dDy = dFdy(D) * coordStepInv.y;
	
	/*
	float dDx = (D_r - D) * coordStepInv.x;
	float dDy = (D_t - D) * coordStepInv.y;
	*/

	float Ex = 0.5 * dzx * dDx - ddzx*D;
	float Ey = 0.5 * dzy * dDy - ddzy*D;

	float H = 0.0;

	if (edge > 0.0)
		H = 0.5 * (Cy*Ex + Cx*Ey) / (pow(D, 1.5));

	frag_color = z_dep + timeStep * H;;
}
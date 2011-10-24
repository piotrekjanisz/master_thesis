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
uniform vec2 coordStepInv;

out float frag_color;

void main()
{
	float edge = texture(edgeTexture, tex_coord).x;
	float edge_r = texture(edgeTexture, tex_coord + vec2(coordStep.x, 0.0)).x;
	float edge_l = texture(edgeTexture, tex_coord - vec2(coordStep.x, 0.0)).x;
	edge = edge * edge_r * edge_l;

	float z = texture(depthTexture, tex_coord).x;
	
	float z_t = texture(depthTexture, tex_coord + vec2(0.0, coordStep.y)).x;
	float z_b = texture(depthTexture, tex_coord + vec2(0.0, -coordStep.y)).x;

	float z_r = texture(depthTexture, tex_coord + vec2(coordStep.x, 0.0)).x;
	float z_l = texture(depthTexture, tex_coord + vec2(-coordStep.x, 0.0)).x;

	float z_rt = texture(depthTexture, tex_coord + vec2(coordStep.x, coordStep.y)).x;
	float z_lt = texture(depthTexture, tex_coord + vec2(-coordStep.x, coordStep.y)).x;

	float z_rb = texture(depthTexture, tex_coord + vec2(coordStep.x, -coordStep.y)).x;
	float z_lb = texture(depthTexture, tex_coord + vec2(-coordStep.x, -coordStep.y)).x;

	float dzdx = 0.5 * Cx * (z_r - z_l);
	float dzdy = 0.5 * Cy * (z_t - z_b);

	float ddzdx2 = Cx * Cx * (z_r - 2*z + z_l);
	float ddzdy2 = Cy * Cy * (z_t - 2*z + z_b);

	float ddzdxdy = 0.25 * Cx * Cy * (z_lb + z_rt - z_lt - z_rb);

	float N = (dzdx*dzdx * ddzdx2 + 2*dzdx*dzdy*ddzdxdy + dzdy*dzdy*ddzdy2) / (dzdx*dzdx + dzdy*dzdy);

	frag_color = z + edge * timeStep * ((ddzdx2 + ddzdy2) - N);
}
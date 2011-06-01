#version 150

in vec2 vTexCoord; 
uniform sampler2D image1; 

out vec4 fragColor;

void main(void)
{
	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//fragColor = texelFetch(image1, ivec2(gl_FragCoord.x * 640, gl_FragCoord.y * 480), 0);
}
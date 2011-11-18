#version 150

uniform vec4 color;
uniform mat4 modelViewProjectionMatrix;

in vec4 vertex;

out vec4 fColor;

void main(void)
{
	gl_Position = modelViewProjectionMatrix * vertex;
	fColor = color;
}
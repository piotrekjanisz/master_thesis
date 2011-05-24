uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
in vec4 vertex;

void main(void)
{
	vec4 v = modelViewMatrix * vertex;

	float len = length(v);

	gl_PointSize = 80.0f / len;

	gl_Position = projectionMatrix * v;
}
#version 150

precision highp float;

layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

in Vertex
{
	in vec3 lightVec;
	in vec3 normal;
	in vec3 tangent;
	in vec3 bitangent;
} vert[];

out vec4 fColor;

uniform mat4	mvpMatrix;
uniform mat4	mvMatrix;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

void main(void)
{
    int n;

    for (n = 0; n < gl_in.length(); n++) {
		// light vector - red
        fColor = vec4(1.0, 0.3, 0.3, 1.0);
        gl_Position = projectionMatrix * gl_in[n].gl_Position;
        EmitVertex();
        fColor = vec4(1.0, 0.3, 0.3, 1.0);
        gl_Position = projectionMatrix * (gl_in[n].gl_Position + vec4(vert[n].normal * 0.5, 0.0));
        EmitVertex();
        EndPrimitive();

		// half vector - green
		fColor = vec4(0.3, 1.0, 0.3, 1.0);
		gl_Position = projectionMatrix * gl_in[n].gl_Position;
		EmitVertex();
		fColor = vec4(0.3, 1.0, 0.3, 1.0);
		gl_Position = projectionMatrix * (gl_in[n].gl_Position + vec4(vert[n].tangent * 0.5, 0.0));
		EmitVertex();
		EndPrimitive();

		fColor = vec4(0.3, 0.3, 1.0, 1.0);
		gl_Position = projectionMatrix * gl_in[n].gl_Position;
		EmitVertex();
		fColor = vec4(0.3, 0.3, 1.0, 1.0);
		gl_Position = projectionMatrix * (gl_in[n].gl_Position + vec4(vert[n].bitangent * 0.5, 0.0));
		EmitVertex();
		EndPrimitive();
    }
}
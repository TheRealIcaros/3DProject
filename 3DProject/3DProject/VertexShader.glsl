#version 430
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertex_tex;

out vec2 texOut;

void main()
{
	texOut = vertex_tex;
	gl_Position = vec4(vertexPosition, 1.0);
}
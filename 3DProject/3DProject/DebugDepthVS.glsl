#version 430
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_tex;

out vec2 TexCoords;

void main()
{
	TexCoords = vertex_tex;
	gl_Position = vec4(vertex_position, 1.0);
}
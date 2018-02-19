#version 430
layout(location = 0) in vec3 vertex_position;
//layout(location = 1) in vec3 vertex_tex;
//layout(location = 1) in vec2 vertex_tex;

out vec3 colorOut;

void main()
{
	//colorOut = vertex_tex;
	gl_Position = vec4(vertex_position, 1.0);
}
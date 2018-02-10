#version 430
layout (location = 0) in vec3 vertex_position;
layout (location = 2) in vec2 vertex_tex;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

out vec3 GeoPos;
out vec2 GeoUV;

void main()
{
	GeoUV = vertex_tex;

	GeoPos = vertex_position;

	//gl_Position = (Projection * View * World) * vec4(vertex_position, 1.0);
}
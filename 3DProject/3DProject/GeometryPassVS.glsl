#version 430
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_tex;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

out vec3 GeoPos;
out vec3 GeoNormal;
out vec2 GeoUV;

void main()
{
	GeoUV = vertex_tex;

	GeoPos = vec4(vertex_position, 1.0).xyz;

	GeoNormal = vertex_normal;
}
#version 430
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_tex;
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec3 vertex_bitangent;

out vec3 GeoPos;
out vec3 GeoNormal;
out vec2 GeoUV;
out vec3 GeoTangent;
out vec3 GeoBitangent;


void main()
{
	GeoUV = vertex_tex;
	GeoPos = vertex_position;
	GeoNormal = vertex_normal;

	GeoTangent = vertex_tangent;
	GeoBitangent = vertex_bitangent;
}
#version 430
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_tex;
layout (location = 3) in vec3 vertex_tangent;
layout (location = 4) in vec3 vertex_bitangent;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

uniform mat4 lightSpaceMatrix;

out vec3 GeoPos;
out vec3 GeoNormal;
out vec2 GeoUV;
out vec3 GeoTangent;
out vec3 GeoBitangent;
out vec4 fragLightSpace;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragUV;


void main()
{
	GeoUV = vertex_tex;
	GeoPos = vertex_position;
	GeoNormal = vertex_normal;
	FragUV = vertex_tex;

	FragPos = (World * vec4(vertex_position, 1.0)).xyz;

	fragLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	FragNormal = mat3(World) * vertex_normal;

	GeoTangent = vertex_tangent;
	GeoBitangent = vertex_bitangent;
}
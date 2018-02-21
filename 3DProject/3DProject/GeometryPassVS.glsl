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

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragUV;
out vec3 FragTangent;
out vec3 FragBitangent;

void main()
{
	FragUV = vertex_tex;
	FragPos = (World * vec4(vertex_position, 1.0)).xyz;
	FragNormal = vertex_normal;
	gl_Position = (Projection * View * World) * vec4(vertex_position, 1.0);

	FragTangent = (World * vec4(vertex_tangent, 0.0)).xyz;
	FragBitangent = (World * vec4(vertex_bitangent, 0.0)).xyz;
}
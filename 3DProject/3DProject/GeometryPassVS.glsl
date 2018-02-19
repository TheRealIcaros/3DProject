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

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragUV;

void main()
{
	FragUV = vertex_tex;

	FragPos = (World * vec4(vertex_position, 1.0)).xyz;

	FragNormal = mat3(World) * vertex_normal;

	gl_Position = (Projection * View * World) * vec4(vertex_position, 1.0);
}
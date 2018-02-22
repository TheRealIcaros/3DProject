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

uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragUV;
out vec4 fragLightSpace;

void main()
{
	FragUV = vertex_tex;

	FragPos = (World * vec4(vertex_position, 1.0)).xyz;

	fragLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

	FragNormal = mat3(World) * vertex_normal;

	gl_Position = (Projection * View * World) * vec4(vertex_position, 1.0);
}
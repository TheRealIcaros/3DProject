#version 430

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 texCoordinates;

uniform mat4 lightspaceMatrix;
uniform mat4 model;


out VertexShader_Data
{
	vec3 fragpos;
	vec3 normal;
	vec2 UV;
	vec4 lightSpaceVector;
}vertexShader_Data;

void main()
{

}
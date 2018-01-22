#version 430
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertex_tex;

//layout(binding = 3, std140) uniform uniformBlock
//{
//	mat4 World;
//mat4 View;
//mat4 Projection;
//};

out vec2 textureCoordinates;

void main()
{
	textureCoordinates = vertex_tex;
	gl_Position = vec4(vertexPosition, 1.0);
}
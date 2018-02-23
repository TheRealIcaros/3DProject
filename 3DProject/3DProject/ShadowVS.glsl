#version 430
layout(location = 0) in vec3 vertex_position;

uniform mat4 lightSpaceMatrix;
uniform mat4 Model;

void main()
{
	gl_Position = (lightSpaceMatrix * Model) * vec4(vertex_position, 1.0);
}
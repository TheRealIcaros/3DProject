#version 430

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;

void main()
{
	float depth = texture(depthMap, TexCoords).r;
	FragColor = vec4(vec3(depth), 1.0); 
}
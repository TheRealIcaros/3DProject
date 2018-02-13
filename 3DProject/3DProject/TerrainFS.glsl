#version 430
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColorSpec;

uniform sampler2D texture_diffuse1;
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragUV;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(FragNormal);
	gColorSpec.rgb = texture(texture_diffuse1, FragUV).rgb;
}
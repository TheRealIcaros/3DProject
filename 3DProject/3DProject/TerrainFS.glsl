#version 430
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColorSpec;
layout(location = 3) out vec4 gColorInfo;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragUV;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(FragNormal);
	gColorSpec.rgb = texture(texture_diffuse1, FragUV).rgb;

	gColorInfo.x = material.ambient.x;
	gColorInfo.y = material.diffuse.x;
	gColorInfo.z = material.specular.x;
	gColorInfo.a = material.shininess;
}
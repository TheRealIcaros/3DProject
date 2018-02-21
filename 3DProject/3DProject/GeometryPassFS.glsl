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
in vec3 FragTangent;
in vec3 FragBitangent;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material;

void main()
{
	//mat3 normalMatrix = transpose(inverse(mat3(World)));
	/*vec3 T = normalize(normalMatrix * FragTangent);
	vec3 B = normalize(normalMatrix * FragBitangent);
	vec3 N = normalize(normalMatrix * FragNormal);*/
	//mat3 TBN = transpose(mat3(T, B, N));
	//mat3 TBN = transpose(mat3(FragTangent, FragBitangent, FragNormal));
	mat3 TBN = mat3(FragTangent, FragBitangent, FragNormal);

	gPosition = FragPos;

	vec3 normalMap = texture(texture_normal1, FragUV).rgb;
	gNormal = normalize(FragNormal + (TBN * normalMap));

	gColorSpec.rgb = texture(texture_diffuse1, FragUV).rgb;


	gColorInfo.x = material.ambient.x;
	gColorInfo.y = material.diffuse.x;
	gColorInfo.z = material.specular.x;
	gColorInfo.a = material.shininess;
}
#version 430
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

in vec2 textureCoordinates;
out vec4 FragColor;

struct Light {
	vec3 Position;
	vec3 Color;
};

vec3 lightPos = vec3(0, 5, 0);

uniform vec3 viewPos;

void main()
{
	//Get Data from gBuffer
	vec3 FragPos = texture(gPosition, textureCoordinates).rgb;
	vec3 Normal = texture(gNormal, textureCoordinates).rgb;
	vec3 Color = texture(gColorSpec, textureCoordinates).rgb;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = -normalize(lightPos - FragPos);
	vec3 reflectDir = reflect(lightDir, Normal);

	//Ambient Light
	float ambient = 0.2f;

	//Diffuse Light
	float diffuse = max(dot(Normal, -lightDir), 0.0);

	//Specular Light
	float specularStrength = 5.0f;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	float specular = spec * specularStrength;

	//Result
	vec3 result = (ambient + diffuse + specular) * Color;

	//FragOut
	FragColor = vec4(result, 1.0);
}
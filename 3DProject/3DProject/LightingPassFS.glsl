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

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

uniform int nrOfLights;
const int lightNr = 16;		//Maximum of 16 lights in light vector array in CPU!!!
uniform Light lights[lightNr];
uniform vec3 viewPos;

void main()
{
	//Get Data from gBuffer
	vec3 FragPos = texture(gPosition, textureCoordinates).rgb;
	vec3 Normal = texture(gNormal, textureCoordinates).rgb;
	vec3 Color = texture(gColorSpec, textureCoordinates).rgb;

	//Ambient Light
	vec3 ambient = material.ambient;

	//Variables
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir;
	vec3 reflectDir;
	vec3 result = ambient * Color;
	
	for (int i = 0; i < nrOfLights; i++)
	{
		lightDir = -normalize(lights[i].Position - FragPos);
		reflectDir = reflect(lightDir, Normal);

		//Diffuse Light
		vec3 diffuse = max(dot(Normal, -lightDir), 0.0) * Color * lights[i].Color;

		//Specular Light
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		vec3 specular = (spec * material.specular) * lights[i].Color;

		//Result
		result += diffuse + specular;
	}

	//FragOut
	FragColor = vec4(result, 1.0);
}
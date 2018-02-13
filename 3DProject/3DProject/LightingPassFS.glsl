#version 430
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

in vec2 textureCoordinates;
//out vec4 FragColor;
layout(location = 0) out vec4 lColor;
layout(location = 1) out vec4 lGlow;


struct Light {
	vec3 Position;
	vec3 Color;
};

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
	float ambient = 0.2;

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
		float specularStrength = 0.5f;
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
		vec3 specular = spec * specularStrength * lights[i].Color;

		//Result
		result += diffuse + specular;
	}

	//FragOut
	lColor = vec4(result, 1.0);

	
	float brightness = dot(lColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
	{
		lGlow = vec4(lColor.rgb, 1.0);
	}
	else
	{
		lGlow = vec4(0.0, 0.0, 0.0, 1.0);
	}

}
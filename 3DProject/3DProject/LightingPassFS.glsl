#version 430
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;
uniform sampler2D gColorInfo;

in vec2 textureCoordinates;
//out vec4 FragColor;
layout(location = 0) out vec4 lColor; //To Merge
layout(location = 1) out vec4 lGlow; //To Gauss

struct Light {
	vec3 Position;
	vec3 Color;
};

uniform int nrOfLights;
const int lightNr = 16;		//Maximum of 16 lights in light vector array in CPU!!!
uniform Light lights[lightNr];
uniform vec3 viewPos;
uniform bool glowKey;
uniform bool intensityKey;

void main()
{
	//Get Data from gBuffer
	vec3 FragPos = texture(gPosition, textureCoordinates).rgb;
	vec3 Normal = texture(gNormal, textureCoordinates).rgb;
	vec3 Color = texture(gColorSpec, textureCoordinates).rgb;

	//ShadowInfo
	float shadow = texture(gNormal, textureCoordinates).a;

	//Material info
	float mAmbient = texture(gColorInfo, textureCoordinates).r;
	float mDiffuse = texture(gColorInfo, textureCoordinates).g;
	float mSpecular = texture(gColorInfo, textureCoordinates).b;
	float mShininess = texture(gColorInfo, textureCoordinates).a;

	//Ambient Light
	float ambient = mAmbient;

	//Variables
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir;
	vec3 reflectDir;
	vec3 result = ambient * Color; //ambient * Color;
	
	for (int i = 0; i < nrOfLights; i++)
	{
		lightDir = normalize(lights[i].Position - FragPos);
		reflectDir = reflect(-lightDir, Normal);

		//Diffuse Light
		vec3 diffuse = (max(dot(Normal, lightDir), 0.0) * Color * mDiffuse) * lights[i].Color;

		//Specular Light
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), mShininess);
		vec3 specular = (spec * mSpecular) * lights[i].Color;

		//Distance
		//float distance = length(lights[i].Position - FragPos);
		//float attenuation = 1.0 / distance;
		//diffuse *= attenuation;
		//specular *= attenuation;

		//Result
		result += ((1.0 - shadow) * (diffuse + specular));
		//result += (diffuse + specular) * Color;
	}
	
	//FragOut
	lColor = vec4(result, 1.0);

	float brightness = dot(lColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (!intensityKey)
	{
		if (!glowKey)
		{
			if (brightness > 1.0) //lower for more glow on everything. Nice test results.
			{
				lGlow = vec4(lColor.rgb, 1.0);
			}
			else
			{
				lGlow = vec4(0.0, 0.0, 0.0, 1.0);
			}

		}
		else
		{
			lGlow = vec4(Color, 1.0);
		}
	}
	else
	{
		if (!glowKey)
		{
			if (brightness > 0.5) //lower for more glow on everything. Nice test results.
			{
				lGlow = vec4(lColor.rgb, 1.0);
			}
			else
			{
				lGlow = vec4(0.0, 0.0, 0.0, 1.0);
			}

		}
		else
		{
			lGlow = vec4(Color, 1.0);
		}
	}
}
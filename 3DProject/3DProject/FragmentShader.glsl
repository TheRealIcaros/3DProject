#version 430
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gColorSpec;

uniform sampler2D texSampler;
in vec2 UV;
in vec3 Normal;
in vec3 FragPos;

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.3f, 1.0f);
	//float ambientLight = 0.1f;				//The ambient-component
	//vec3 lightPosition = vec3(0, 0, 5);	//The position of the light in the scene
	//vec3 lightDirection = normalize(lightPosition - worldPosition.xyz);		//The direction the light is faceing
	//float diffuse = clamp(dot(normal.xyz, lightDirection), 0, 1);	//The diffuse-component
	//FragColor  =  vec4(texture(texSampler, UV).rgb, 0.2);
	//fragment_color = mySample * (ambientLight + diffuse);
	//vec4 mySample = vec4(texture(texSampler, UV).rgb, 1);
	//FragColor = mySample; // *(ambientLight + diffuse);

	gPosition = FragPos.xyz;
	gNormal = normalize(Normal);
	gColorSpec.rgb = texture(texSampler, UV).rgb;
	//gColorSpec.a = texture(texSampler, UV).r;
}
#version 430
uniform sampler2D texSampler;
in vec2 UV;
in vec4 normal;
in vec4 worldPosition;
out vec4 FragColor;

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.3f, 1.0f);

	float ambientLight = 0.1f;				//The ambient-component
	vec3 lightPosition = vec3(0, 0, 5);	//The position of the light in the scene
	vec3 lightDirection = normalize(lightPosition - worldPosition.xyz);		//The direction the light is faceing

	float diffuse = clamp(dot(normal.xyz, lightDirection), 0, 1);	//The diffuse-component

	FragColor  =  vec4(texture(texSampler, UV).rgb, 0.2);
	//vec4 mySample = vec4(texture(texSampler, UV).rgb, 0);
	//FragColor = mySample * (ambientLight + diffuse);
	//fragment_color = mySample * (ambientLight + diffuse);
}
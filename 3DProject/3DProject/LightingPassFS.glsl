#version 430
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

in vec2 textureCoordinates;
out vec4 FragColor;

struct Light {
	vec3 Position;
	vec3 Color;

	float Linear;
	float Quadratic;
	float Radius;
};

vec3 lightPos = vec3(0, 0, 5);
uniform vec3 viewPos;

void main()
{
	//FragColor = vec4(1.0f, 0.5f, 0.3f, 1.0f);
	//
	//float ambientLight = 0.1f;				//The ambient-component
	//vec3 lightPosition = vec3(0, 0, 5);	//The position of the light in the scene
	//vec3 lightDirection = normalize(lightPosition - worldPosition.xyz);		//The direction the light is faceing
	//
	//float diffuse = clamp(dot(normal.xyz, lightDirection), 0, 1);	//The diffuse-component
	//
	//FragColor  =  vec4(texture(texSampler, UV).rgb, 0.2);
	//
	//vec4 mySample = vec4(texture(texSampler, UV).rgb, 0);
	//FragColor = mySample * (ambientLight + diffuse);
	//fragment_color = mySample * (ambientLight + diffuse);

	// get data from g-buffer
	vec3 FragPos = texture(gPosition, textureCoordinates).rgb;
	vec3 Normal = texture(gNormal, textureCoordinates).rgb;
	vec3 Albedo = texture(gColorSpec, textureCoordinates).rgb;
	float Specular = texture(gColorSpec, textureCoordinates).a;

	// Calculate lighting as usual
	vec3 lighting = Albedo * 0.1;
	vec3 viewDir = normalize(viewPos - FragPos);
	
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo;
	lighting += diffuse;
	
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	FragColor = vec4(lighting, 1.0);
}
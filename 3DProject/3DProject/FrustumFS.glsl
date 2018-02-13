#version 430
//uniform sampler2D texSampler;
in vec3 inColor;
in vec4 normal;
in vec4 worldPos;
out vec4 fragment_color;

void main()
{
	float ambientStrength = 1.0;

	//float cosTheta = clamp(dot(normal.xyz , vec3(0,0,-5)),0,1);
	vec3 lightPos = vec3(0, 0, 5);
	vec3 lightDir = normalize(lightPos - worldPos.xyz);
	//float diffuse = max(dot(normal.xyz, lightDir), 0.0);
	float diffuse = clamp(dot(normal.xyz, lightDir), 0, 1);


	vec4 mySample = vec4(inColor, 1.0);/*texture(texSampler, vec2(UV.s, UV.t));*/
	fragment_color = mySample * (ambientStrength + diffuse);
	
	//fragment_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

}
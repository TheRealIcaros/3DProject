#version 430
out vec4 FragColor;

in vec2 textureCoordinates;



uniform sampler2D lColor;
uniform sampler2D lGlow;
uniform bool bloomKey;

void main()
{
	const float exposure = 1.0f;
	const float gamma = 1.0;

	vec3 hdrColor = texture(lColor, textureCoordinates).rgb;
	vec3 bloomColor = texture(lGlow, textureCoordinates).rgb;
	hdrColor += bloomColor;

	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	result = pow(result, vec3(1.0 / gamma));
	if (!bloomKey)
	{
		FragColor = vec4(result, 1.0);
	}
	else
	{
		FragColor = vec4(bloomColor, 1.0);
	}
	//FragColor = vec4(bloomColor + hdrColor, 1.0);
	//FragColor = vec4(bloomColor, 1.0);
}


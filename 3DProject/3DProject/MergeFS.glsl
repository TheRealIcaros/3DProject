#version 430

in vec2 texureCoordinates;

out vec4 FragColor;

uniform sampler2D lColor;
uniform sampler2D lGlow;
uniform float exposure;

void main()
{
	const float gamma = 2.2;

	vec3 hdrColor = texture(lColor, texureCoordinates).rgb;
	vec3 bloomColor = texture(lGlow, texureCoordinates).rgb;
	hdrColor += bloomColor;

	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	result = pow(result, vec3(1.0 / gamma));
	FragColor = vec4(result, 1.0);
	FragColor = vec4(hdrColor, 1.0);
}


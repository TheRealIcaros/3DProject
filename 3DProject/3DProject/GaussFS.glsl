#version 430 core
out vec4 FragColor;

in vec2 textureCoordinates;

uniform sampler2D inputValue;

uniform int horizontal;
const int kernelSize = 9;
uniform float weight[kernelSize] = float[](
	0.073441,
	0.072697,
	0.070509,
	0.067007,
	0.062396,
	0.05693	,
	0.050895,	
	0.044582,
	0.038265
	);

//Kernel weight found here
//http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{
	vec2 tex_offset = 1.0 / textureSize(inputValue, 0); // gets the size of the texture pixels
	vec3 result = texture(inputValue, textureCoordinates).rgb * weight[0]; // current fragment's contribution
	if (horizontal == 0)
	{
		for (int i = 1; i < kernelSize; i++)
		{
			result += texture(inputValue, textureCoordinates + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(inputValue, textureCoordinates - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < kernelSize; i++)
		{
			result += texture(inputValue, textureCoordinates + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(inputValue, textureCoordinates - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	FragColor = vec4(result, 1.0);
}
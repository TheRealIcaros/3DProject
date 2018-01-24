#version 430
layout(triangles)in;
layout(triangle_strip, max_vertices = 6) out;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

vec3 getNormal(vec3 inNormal)
{
	//vec3 U = (gl_Position.[1].xyz - gl_Position.[0].xyz);
	//vec3 V = (gl_Position.[2].xyz - gl_Position.[1].xyz);

	//Normal.x = (U.y * V.z) - (U.z * V.y);
	//Normal.y = (U.z * V.x) - (U.x * V.z);
	//Normal.z = (U.x - V.y) - (U.y - V.x);
	
	//Normal = normalize(Normal);

	//return Normal;

	vec3 U = (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	vec3 V = (gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz);
	//
	inNormal.x = (U.y * V.z) - (U.z * V.y);
	inNormal.y = (U.z * V.x) - (U.x * V.z);
	inNormal.z = (U.x - V.y) - (U.y - V.x);
	//
	inNormal = normalize(inNormal);
	//
	return inNormal;
}

in vec2 texOut[];
out vec2 UV;
out vec3 Normal;
out vec3 FragPos;

void main()
{
	vec3 normal;
	normal = getNormal(normal);

	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = (Projection * View * World) * gl_in[i].gl_Position;
		UV = texOut[i];
		
		Normal = (World * vec4(normal, 0.0)).xyz;

		FragPos = (World * gl_in[i].gl_Position).xyz;

		EmitVertex();
	}
	EndPrimitive();

	/*for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = (Projection * View * World) * (gl_in[i].gl_Position + vec4(3.0, 0.0, 0.0, 0.0));
		
		UV = texOut[i];

		Normal = (World * vec4(normal, 0.0)).xyz;

		FragPos = (World * gl_in[i].gl_Position + vec4(normal, 1.0)).xyz;

		EmitVertex();
	}
	EndPrimitive();*/
}
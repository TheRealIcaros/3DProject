#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 6) out;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

vec3 getNormal(vec3 Normal)
{
	vec3 U = (gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz);
	vec3 V = (gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz);

	Normal.x = (U.y * V.z) - (U.z * V.y);
	Normal.y = (U.z * V.x) - (U.x * V.z);
	Normal.z = (U.x - V.y) - (U.y - V.x);

	Normal = normalize(Normal);

	return Normal;
}

in vec2 texOut[];
out vec2 UV;
out vec4 normal;
out vec4 worldPos;

void main()
{
	vec3 Normal;
	Normal = getNormal(Normal);
	//normal = Normal;

	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = (Projection * View * World) * gl_in[i].gl_Position;
		UV = texOut[i];

		normal = World * vec4(Normal, 1.0);

		worldPos = World * gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();

	/*for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = (Projection * View * World) * (gl_in[i].gl_Position + vec4(Normal, 0.0));
		UV = texOut[i];

		normal = World * vec4(Normal, 1.0);

		worldPos = World * (gl_in[i].gl_Position + vec4(Normal, 0.0));
		EmitVertex();
	}
	EndPrimitive();*/
}
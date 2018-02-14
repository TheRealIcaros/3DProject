#version 430
layout(triangles)in;
layout(triangle_strip, max_vertices = 3) out;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

vec3 calculateNormal(vec3 pos0, vec3 pos1, vec3 pos2)
{
	vec3 edge0 = pos0 - pos1;
	vec3 edge1 = pos2 - pos1;

	return normalize(cross(edge0, edge1));;
}

in vec3 GeoPos[];
in vec2 GeoUV[];

out vec2 FragUV;
out vec3 FragNormal;
out vec3 FragPos;

void main()
{
	vec3 normal = calculateNormal(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);

	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = (Projection * View * World) * vec4(gl_in[i].gl_Position.xyz, 1.0);

		FragUV = GeoUV[i];
		
		FragNormal = (World * vec4(-normal, 0.0)).xyz;
		
		FragPos = (World * gl_in[i].gl_Position).xyz;

		EmitVertex();
	}
	EndPrimitive();
}
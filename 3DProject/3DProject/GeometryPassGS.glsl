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

uniform vec3 cameraPos;

in vec3 GeoPos[];
in vec2 GeoUV[];
in vec3 GeoNormal[];

out vec2 FragUV;
out vec3 FragNormal;
out vec3 FragPos;

void main()
{
	vec3 thisNormal = calculateNormal(GeoPos[0], GeoPos[1], GeoPos[2]);
	for (int i = 0; i < gl_in.length(); i++)
	{
		vec3 cameraDir = GeoPos[i].xyz - cameraPos;
		if (dot(thisNormal, cameraDir) > -0.001f)
		{
			gl_Position = (Projection * View * World) * vec4(GeoPos[i].xyz, 1.0);

			FragUV = GeoUV[i];

			FragNormal = (World * vec4(GeoNormal[i], 0.0)).xyz;

			FragPos = (World * gl_in[i].gl_Position).xyz;

			EmitVertex();
		}
	}
	EndPrimitive();
}
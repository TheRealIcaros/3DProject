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
uniform mat4 Model;

in vec3 GeoPos[];
in vec2 GeoUV[];
in vec3 GeoNormal[];
in vec3 GeoTangent[];
in vec3 GeoBitangent[];

out vec2 FragUV;
out vec3 FragNormal;
out vec3 FragPos;
out vec3 FragTangent;
out vec3 FragBitangent;

void main()
{
	vec3 thisNormal = calculateNormal(GeoPos[0], GeoPos[1], GeoPos[2]);
	for (int i = 0; i < gl_in.length(); i++)
	{

		vec3 cameraDir = ((Model) * vec4(GeoPos[i].xyz, 1.0)).xyz - cameraPos;
		if (dot(thisNormal, cameraDir) > -0.001f)
		{
			gl_Position = (Projection * View * Model) * vec4(GeoPos[i].xyz, 1.0);

			FragUV = GeoUV[i];

			FragTangent = (Model * vec4(GeoTangent[i], 0.0)).xyz;

			FragBitangent = (Model * vec4(GeoBitangent[i], 0.0)).xyz;

			FragNormal = (Model * vec4(GeoNormal[i], 0.0)).xyz;

			FragPos = (Model * gl_in[i].gl_Position).xyz;

			EmitVertex();
		}
	}
	EndPrimitive();
}
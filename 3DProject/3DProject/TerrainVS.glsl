#version 430
layout (location = 0) in vec3 vertex_position;
layout (location = 2) in vec2 vertex_tex;

layout(binding = 3, std140) uniform uniformBlock
{
	mat4 World;
	mat4 View;
	mat4 Projection;
};

out vec3 GeoPos;
out vec2 GeoUV;

uniform vec3 cameraPos;

void main()
{
	GeoUV = vertex_tex;
	gl_Position = vec4(vertex_position, 1.0);

	//This is test :D
	/*float Curvature = 0.01f;
	vec4 vv = (World) * vec4(vertex_position, 1.0);
	vv.xyz -= cameraPos.xyz;
	vv = vec4(0.0f, (vv.x * vv.z) * -Curvature, 0.0f, 1.0f);
	gl_Position += vv * inverse(World);*/
}
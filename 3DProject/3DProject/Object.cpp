#include "Object.h"

Object::Object()
{

}

Object::~Object()
{
}

void Object::loadObject(const char* objPath, vec3 startPosition)
{
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	vector<Material> materials;
	this->objLoader.loadOBJ(objPath, vertices, uvs, normals, materials);

	vector<Vertex> vertexes;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex temp;
		temp.Position = vertices[i] + startPosition;
		temp.Normal = normals[i];
		temp.TexCoords = uvs[i];
		vertexes.push_back(temp);
	}

	vector<unsigned int> indices;
	for (unsigned int i = 0; i < vertexes.size() * 3; i++)
	{
		indices.push_back(i);
	}

	meshes.push_back(Mesh(vertexes, indices, materials, startPosition));
}

void Object::Draw(ShaderCreater shader)
{
	for (int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].Draw(shader);
	}
}

void Object::DrawDepth(ShaderCreater shader)
{
	for (int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].DrawDepth(shader);
	}
}
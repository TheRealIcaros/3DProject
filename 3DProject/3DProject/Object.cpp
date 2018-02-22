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
	vector<vec3> tangents;
	vector<Material> materials;
	this->objLoader.loadOBJ(objPath, vertices, uvs, normals, tangents, materials);

	vector<Vertex> vertexes;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex temp;
		temp.Position = vertices[i];
		temp.Normal = normals[i];
		temp.TexCoords = uvs[i];
		temp.Tangent = tangents[i];
		temp.Bitangent = cross(normals[i], tangents[i]);
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

void Object::Sort(vec3 cameraPos)
{
	bool sorted = false;
	int nrModels = this->meshes.size();
	vector<float> distances(nrModels);
	vec3 cam_pos = cameraPos;

	for (int i = 0; i < nrModels; i++)
	{
		distances[i] = distance(cam_pos, meshes[i].getModelPosition());
	}

	while (!sorted)
	{
		sorted = true;
		for (int i = 0; i < nrModels - 1; i++)
		{
			if (distances[i] > distances[i + 1])
			{
				swap(meshes[i], meshes[i + 1]);
				swap(distances[i], distances[i + 1]);
				sorted = false;
			}
		}
	}
}
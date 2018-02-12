#include "Object.h"

Object::Object()
{

}

Object::~Object()
{
}

void Object::loadObject(const char* objPath, const char* texturePath, vec3 startPosition)
{
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	this->objLoader.loadOBJ(objPath, vertices, uvs, normals);

	vector<Vertex> vertexes;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex temp;
		temp.Position = vertices[i];
		temp.Normal = normals[i];
		temp.TexCoords = uvs[i];
		vertexes.push_back(temp);
	}

	vector<unsigned int> indices;
	for (unsigned int i = 0; i < vertexes.size() * 3; i++)
	{
		indices.push_back(i);
	}

	vector<Texture> textures;
	Texture texture;
	texture.id = this->objLoader.TextureFromFile(texturePath);
	texture.type = "texture_diffuse";
	texture.path = texturePath;
	textures.push_back(texture);

	meshes.push_back(Mesh(vertexes, indices, textures, startPosition));
}

void Object::Draw(ShaderCreater shader)
{
	for (int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].Draw(shader);
	}
}
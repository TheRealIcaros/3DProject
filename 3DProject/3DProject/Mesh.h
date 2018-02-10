#ifndef MESH_H
#define MESH_H

#include "Defines.h"
#include <vector>

//Own Classes
#include "ShaderCreater.h"

using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
private:
	glm::vec3 localPosition;
	unsigned int VAO, VBO, EBO;
	void setupMesh();
public:
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, glm::vec3 startPosition);
	~Mesh();

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	void Draw(ShaderCreater shader);
};

#endif // !MESH_H

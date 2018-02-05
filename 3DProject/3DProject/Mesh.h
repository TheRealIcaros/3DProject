#ifndef MESH_H
#define MESH_H

#include "Defines.h"
#include <vector>
#include <assimp\Importer.hpp>

//Own Classes
#include "ShaderCreater.h"

using namespace std;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	/*glm::vec3 Tangent;
	glm::vec3 Bitangent;*/
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
	unsigned int VAO, VBO, EBO;
	void setupMesh();
public:
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	~Mesh();

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	void Draw(ShaderCreater shader);
};

#endif // !MESH_H

#ifndef MESH_H
#define MESH_H

#include "Defines.h"
#include <vector>

//Own Classes
#include "ShaderCreater.h"

using namespace std;
using namespace glm;

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};

struct Texture
{
	unsigned int id;
	string type;
	string path;
};

struct Material
{
	string name;

	unsigned int id;
	string type;
	string path;

	vec3 colorAmbient;
	vec3 colorDiffuse;
	vec3 colorSpecular;
	float specularExponent;
};

class Mesh
{
private:
	vec3 localPosition;
	unsigned int VAO, VBO, EBO;
	void setupMesh();
public:
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Material> materials, vec3 startPosition);
	~Mesh();

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Material> materials;

	void Draw(ShaderCreater shader);
	void sendMaterials(ShaderCreater shader)const;
};

#endif // !MESH_H

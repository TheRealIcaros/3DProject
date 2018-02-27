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
	vec3 Tangent;
	vec3 Bitangent;
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

	vector<Texture> textures;

	vec3 colorAmbient;
	vec3 colorDiffuse;
	vec3 colorSpecular;
	float specularExponent;
};

class Mesh
{
private:
	mat4 Model;
	vec3 localPosition;
	unsigned int VAO, VBO, EBO;
	void setupMesh();
public:
	Mesh();
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Material> materials, vec3 startPosition);
	~Mesh();

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Material> materials;

	vec3 getModelPosition();

	void Draw(ShaderCreater shader);
	void DrawDepth(ShaderCreater shader);
	void deallocate();
};

#endif // !MESH_H

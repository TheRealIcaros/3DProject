#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Defines.h"
#include <vector>

using namespace std;
using namespace glm;

class OBJLoader
{
private:

public:
	OBJLoader();
	~OBJLoader();

	bool loadOBJ(const char* path, vector<vec3> &vertices, vector<vec2> &uvs, vector<vec3> &normals);
};

#endif

OBJLoader::OBJLoader()
{
}

OBJLoader::~OBJLoader()
{
}

bool OBJLoader::loadOBJ(const char* path, vector<vec3> &vertices, vector<vec2> &uvs, vector<vec3> &normals)
{
	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> tempVertices;
	vector<vec2> tempUVs;
	vector<vec3> tempNormals;

	FILE * file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Can't open the file: %s", path);
		return false;
	}

	while (true)
	{
		char fileLine[128];

		int result = fscanf(file, "%s", fileLine);
		if (result == EOF)
			break;

		if (strcmp(fileLine, "v") == 0)
		{
			vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			tempVertices.push_back(vertex);
		}
		else if (strcmp(fileLine, "vt") == 0)
		{
			vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			tempUVs.push_back(uv);
		}
		else if (strcmp(fileLine, "vn") == 0)
		{
			vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			tempNormals.push_back(normal);
		}
		else if (strcmp(fileLine, "f") == 0)
		{
			string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			int batch = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (batch != 9)
			{
				printf("File can't be read!");
				return false;
			}

			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);

			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);

			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	//Vertices
	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		vec3 vertex = tempVertices[vertexIndex - 1];
		vertices.push_back(vertex);
	}

	//UVs
	for (unsigned int i = 0; i < uvIndices.size(); i++)
	{
		unsigned int uvIndex = uvIndices[i];
		vec2 uv = tempUVs[uvIndex - 1];
		uvs.push_back(uv);
	}

	//Normals
	for (unsigned int i = 0; i < normalIndices.size(); i++)
	{
		unsigned int normalIndex = normalIndices[i];
		vec3 normal = tempNormals[normalIndex - 1];
		normals.push_back(normal);
	}
}

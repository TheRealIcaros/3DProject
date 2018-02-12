#include "OBJLoader.h"

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
		char fileLine[512];
		char *pos = nullptr;

		int result = fscanf(file, "%s", fileLine);
		if (result == EOF)
			break;

		if (strcmp(fileLine, "mtllib") == 0)
		{
			char temp[128], mtl[128];
			sscanf(fileLine, "%s %s\n", &temp, &mtl);
			//readMTL(mtl);
		}
		else if (strcmp(fileLine, "v") == 0)
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

bool OBJLoader::loadMTL()
{
	return false;
}

unsigned int OBJLoader::TextureFromFile(const char* texturePath)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = SOIL_load_image(texturePath, &width, &height, &nrComponents, SOIL_LOAD_AUTO);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGBA;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << texturePath << std::endl;
		SOIL_free_image_data(data);
	}

	return textureID;
}

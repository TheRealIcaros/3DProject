#include "OBJLoader.h"

OBJLoader::OBJLoader()
{
}

OBJLoader::~OBJLoader()
{
}

bool OBJLoader::loadOBJ(const char* objPath, vector<vec3> &vertices, vector<vec2> &uvs, vector<vec3> &normals, vector<vec3> &tangent, vector<Material> &materials)
{
	string directory = objPath;
	directory = directory.substr(0, directory.find_last_of("/") + 1);

	vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	vector<vec3> tempVertices;
	vector<vec2> tempUVs;
	vector<vec3> tempNormals;

	FILE * file = fopen(objPath, "r");
	if (file == NULL)
	{
		printf("Can't open the file: %s", objPath);
		return false;
	}

	while (true)
	{
		char fileLine[512];

		int result = fscanf(file, "%s", fileLine);
		if (result == EOF)
			break;

		if (strcmp(fileLine, "mtllib") == 0)
		{
			char mtl[128];
			fscanf(file, "%s", &mtl);
			loadMTL(directory.c_str(), mtl, materials);
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
			uv.y = 1.0 - uv.y;
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

	//Tangent
	for (unsigned int i = 0; i < vertices.size(); i += 3)
	{
		vec3 p0 = vertices[i];
		vec3 p1 = vertices[i + 1];
		vec3 p2 = vertices[i + 2];

		vec2 t0 = uvs[i];
		vec2 t1 = uvs[i + 1];
		vec2 t2 = uvs[i + 2];

		vec3 deltaPos1 = p1 - p0;
		vec3 deltaPos2 = p2 - p0;

		vec2 deltaUv1 = t1 - t0;
		vec2 deltaUv2 = t2 - t0;

		float r = 1 / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);

		vec3 tempTangent;
		tempTangent.x = r * (deltaUv2.y * deltaPos1.x - deltaUv1.y * deltaPos2.x);
		tempTangent.y = r * (deltaUv2.y * deltaPos1.y - deltaUv1.y * deltaPos2.y);
		tempTangent.z = r * (deltaUv2.y * deltaPos1.z - deltaUv1.y * deltaPos2.z);
		tempTangent = normalize(tempTangent);		
		
		tangent.push_back(tempTangent);
		tangent.push_back(tempTangent);
		tangent.push_back(tempTangent);
	}

	return true;
}

bool OBJLoader::loadMTL(const char* path, const char* mtlFile, vector<Material> &materials)
{
	string mtlPath = (string)path + (string)mtlFile;
	FILE * file = fopen(mtlPath.c_str(), "r");
	if (file == NULL)
	{
		printf("Can't open the file: %s", mtlFile);
		return false;
	}

	Material tempMaterial;

	while (true)
	{
		char fileLine[512];

		int result = fscanf(file, "%s", fileLine);
		if (result == EOF)
		{
			if (tempMaterial.name != "")
				materials.push_back(tempMaterial);
			break;
		}

		if (strcmp(fileLine, "newmtl") == 0)
		{
			if (tempMaterial.name != "")
			{
				materials.push_back(tempMaterial);
				tempMaterial.name = "";
			}

			char temp[128];
			fscanf(file, "%s", &temp);
			tempMaterial.name = temp;
		} 
		else if (strcmp(fileLine, "Ns") == 0)
		{
			float temp;
			fscanf(file, "%f", &temp);
			tempMaterial.specularExponent = temp;
		}
		else if (strcmp(fileLine, "Ka") == 0)
		{
			vec3 temp;
			fscanf(file, "%f %f %f", &temp.x, &temp.y, &temp.z);
			tempMaterial.colorAmbient = temp;
		}
		else if (strcmp(fileLine, "Kd") == 0)
		{
			vec3 temp;
			fscanf(file, "%f %f %f", &temp.x, &temp.y, &temp.z);
			tempMaterial.colorDiffuse = temp;
		}
		else if (strcmp(fileLine, "Ks") == 0)
		{
			vec3 temp;
			fscanf(file, "%f %f %f", &temp.x, &temp.y, &temp.z);
			tempMaterial.colorSpecular = temp;
		}
		else if (strcmp(fileLine, "map_Kd") == 0)
		{
			char name[128];
			fscanf(file, "%s", &name);
			string png = (string)path + (string)name;

			Texture tempTexture;
			tempTexture.id = TextureFromFile(png.c_str());
			tempTexture.path = path;
			tempTexture.type = "texture_diffuse";
			tempMaterial.textures.push_back(tempTexture);
		}
		else if (strcmp(fileLine, "map_bump") == 0)
		{
			char name[128];
			fscanf(file, "%s", &name);
			string png = (string)path + (string)name;

			Texture tempTexture;
			tempTexture.id = TextureFromFile(png.c_str());
			tempTexture.path = path;
			tempTexture.type = "texture_normal";
			tempMaterial.textures.push_back(tempTexture);
		}
	}

	return true;
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

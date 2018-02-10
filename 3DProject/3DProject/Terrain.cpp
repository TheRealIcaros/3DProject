#include "Terrain.h"
//#include <stb_image.h>

Terrain::Terrain()
{

}

Terrain::Terrain(vec3 startPosition, const char *heightMapPath, string texturePath)
{
	this->maxHeight = 0.01f;
	this->terrainPosition = startPosition;
	//this->texturePath = texturePath;
	this->imageData = loadHeightMap(heightMapPath);

	createTerrain();
}

Terrain::~Terrain()
{

}

unsigned char* Terrain::loadHeightMap(const char *path)
{
	int nrChannels;
	unsigned char* data = stbi_load(path, &this->imageWidth, &this->imageHeight, &nrChannels, 0);
	if(!data)
	{
		std::cout << "Failed to load height map" << endl;
		data = NULL;
	}

	return data;
}

void Terrain::createTerrain()
{
	int index = 0;
	for (int z = 0; z < this->imageHeight ; z++)
	{
		for (int x = 0; x < this->imageWidth; x++)
		{
			float gray = ((float)this->imageData[index] +
				(float)this->imageData[index + 1] +
				(float)this->imageData[index + 2]) / 3;

			index += 3;

			gray = gray * this->maxHeight;
			vec3 position;
			position.x = x;
			position.y = gray;
			position.z = z;
			vertices.push_back(position + this->terrainPosition);
			//printf("x: %f, y: %f, z: %f\n", position.x, position.y, position.z);
		}
		//printf("\n");
	}
	triangulate();
	createNormalMap();
	sendToModel();
}

void Terrain::createNormalMap()
{
	//for (int i = 0; i < this->imageHeight; i++)
	//{
	//	for (int j = 0; j < this->imageWidth; j++)
	//	{
	//		float Z1 = getPixelColor(vec2(i + 1, j)) - getPixelColor(vec2(i - 1, j));
	//		float Z2 = getPixelColor(vec2(i, j + 1)) - getPixelColor(vec2(i, j - 1));
	//
	//		vec3 A(1.0, 0.0, Z1);
	//		vec3 B(0.0, 1.0, Z2);
	//		vec3 N = normalize(cross(A, B)); //(B.z, A.z, 1);
	//
	//		normals.push_back(normalize(N));
	//	}
	//}

	for (int i = 0; i < this->indices.size(); i += 3)
	{
		vec3 pos0 = this->vertices[this->indices[i]];
		vec3 pos1 = this->vertices[this->indices[i + 1]];
		vec3 pos2 = this->vertices[this->indices[i + 2]];

		vec3 edge0 = pos0 - pos1;
		vec3 edge1 = pos2 - pos1;

		vec3 normal = normalize(cross(edge0, edge1));;

		this->normals.push_back(normal);
	}
}

void Terrain::triangulate()
{
	for (int z = 0; z < this->imageHeight - 1; z++)
	{
		for (int x = 0; x < this->imageWidth - 1; x++)
		{		
			//First triangle
			this->indices.push_back(this->imageWidth * z + x);
			this->indices.push_back((this->imageWidth * z + x) + this->imageWidth);
			this->indices.push_back((this->imageWidth * z + x) + 1);

			//Second triangle
			this->indices.push_back((this->imageWidth * z + x) + 1);
			this->indices.push_back((this->imageWidth * z + x) + this->imageWidth);
			this->indices.push_back((this->imageWidth * z + x) + this->imageWidth + 1);

			/*printf("p0: %d, p1: %d, p2: %d\n", triangles[index], triangles[index + 1], triangles[index + 2]);
			printf("p0: %d, p1: %d, p2: %d\n\n", triangles[index + 3], triangles[index + 4], triangles[index + 5]);*/
		}
	}
}

float Terrain::getPixelColor(vec2 pos)
{
	vec3 color;

	color.r = (float)this->imageData[(int)pos.y * this->imageWidth + (int)pos.x];
	color.g = (float)this->imageData[(int)pos.y * this->imageWidth + (int)pos.x + 1];
	color.b = (float)this->imageData[(int)pos.y * this->imageWidth + (int)pos.x + 2];

	return (color.r + color.g + color.b) / 3;
}

void Terrain::sendToModel()
{
	vector<Vertex> outData;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex temp;
		temp.Position = vertices[i];
		temp.Normal = vec3();
		temp.TexCoords = vec2(0.0, 0.0);
		outData.push_back(temp);
	}

	this->terrain = Model(outData, this->indices);
}

void Terrain::Draw(ShaderCreater shader)
{
	this->terrain.Draw(shader);
}
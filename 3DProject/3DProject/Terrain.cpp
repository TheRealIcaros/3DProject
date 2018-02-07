#include "Terrain.h"

Terrain::Terrain()
{

}

Terrain::Terrain(vec3 startPosition, const char *path)
{
	this->maxHeight = 8;
	this->minHeight = 0;
	this->terrainPosition = startPosition;
	this->imageData = loadHeightMap(path);

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
	for (int z = 0; z < this->imageHeight ; z++)
	{
		for (int x = 0; x < this->imageWidth; x++)
		{
			float gray = ((float)this->imageData[z * this->imageWidth + x] +
				(float)this->imageData[z * this->imageWidth + x + 1] +
				(float)this->imageData[z * this->imageWidth + x + 2]) / 3;

			gray = clamp(gray, this->minHeight, this->maxHeight);
			vec3 position;
			position.x = x;
			position.y = gray;
			position.z = z;
			vertices.push_back(position);
			//printf("x: %f, y: %f, z: %f\n", position.x, position.y, position.z);
		}
		//printf("\n");
	}
	triangulate();
}

vec3 Terrain::calculateNormal(vec3 p0, vec3 p1, vec3 p2)
{
	vec3 e0 = p0 - p1;
	vec3 e1 = p2 - p1;

	vec3 normal = normalize(cross(e0, e1));

	return normal;
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

	vector<Vertex> outData;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex temp;
		temp.Position = vertices[i];
		outData.push_back(temp);
	}

	this->terrain = Model(outData, this->indices);
}

void Terrain::Draw(ShaderCreater shader)
{
	this->terrain.Draw(shader);
}
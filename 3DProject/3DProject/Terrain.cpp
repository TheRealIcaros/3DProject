#include "Terrain.h"

Terrain::Terrain()
{
	this->height = 5;
	this->width = 3;
	this->terrainPosition = vec3(0.0, 0.0, 0.0);

	//this->vertices = new vec3[this->height * this->width];
	this->triangles = new int[(this->height * this->width) * 3];

	createTerrain();
}

Terrain::Terrain(int height, int width, vec3 startPosition)
{
	this->height = height;
	this->width = width;
	this->terrainPosition = startPosition;

	//this->vertices = new vec3[this->height * this->width];
	this->triangles = new int[(this->height * this->width) * 3];

	createTerrain();
}

Terrain::~Terrain()
{
	//delete[] this->vertices;
	delete[] this->triangles;
}

void Terrain::createTerrain()
{
	int index = 0;
	for (int z = 0; z < this->height; z++)
	{
		for (int x = 0; x < this->width; x++)
		{
			this->vertices.push_back(vec3((float)x, 0.0, (float)z));

			printf("x: %f, z: %f\n", this->vertices[index].x, this->vertices[index].z);
			index++;
		}
	}
}

void Terrain::triangulate()
{
	int index = 0;
	for (int z = 0; z < this->height - 1; z++)
	{
		for (int x = 0; x < this->width - 1; x++)
		{		
			//First triangle
			this->triangles[index] = (this->width * z + x);
			this->triangles[index + 1] = (this->width * z + x) + this->width;
			this->triangles[index + 2] = (this->width * z + x) + 1;

			//Second triangle
			this->triangles[index + 3] = (this->width * z + x) + 1;
			this->triangles[index + 4] = (this->width * z + x) + this->width;
			this->triangles[index + 5] = (this->width * z + x) + this->width + 1;

			/*printf("p0: %d, p1: %d, p2: %d\n", triangles[index], triangles[index + 1], triangles[index + 2]);
			printf("p0: %d, p1: %d, p2: %d\n\n", triangles[index + 3], triangles[index + 4], triangles[index + 5]);*/

			index += 6;
		}
	}
}
#ifndef TERRAIN_H
#define TERRAIN_H

#include "ShaderCreater.h"
#include "Model.h"
#include <vector>
#include "Defines.h"

using namespace glm;
using namespace std;

class Terrain
{
private:
	vec3 terrainPosition;

	float maxHeight;
	float minHeight;

	unsigned int heightMapID;
	unsigned char* imageData;
	int imageHeight;	//X-axis
	int imageWidth;		//Z-axis

	vector<vec3> vertices;
	vector<unsigned int> indices;

	Model terrain;
public:
	Terrain();
	Terrain(vec3 startPosition, const char *path);
	~Terrain();

	unsigned char* loadHeightMap(const char *path);
	void createTerrain();
	vec3 calculateNormal(vec3 p0, vec3 p1, vec3 p2);
	void triangulate();

	void Draw(ShaderCreater shader);
};

#endif // Terrain

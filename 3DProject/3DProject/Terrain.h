#ifndef TERRAIN_H
#define TERRAIN_H

#include "ShaderCreater.h"
#include <vector>

using namespace glm;
using namespace std;

class Terrain
{
private:
	vec3 terrainPosition;

	int height;	//X-axis
	int width;	//Z-axis

	vector<vec3> vertices;
	int *triangles;
public:
	Terrain();
	Terrain(int height, int width, vec3 startPosition);
	~Terrain();

	void createTerrain();
	void triangulate();
};

#endif // Terrain

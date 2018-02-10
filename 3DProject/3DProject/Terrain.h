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

	string texturePath;
	unsigned int heightMapID;
	unsigned char* imageData;
	int imageHeight;	//X-axis
	int imageWidth;		//Z-axis

	vector<vec3> vertices;
	vector<vec3> normals;
	vector<unsigned int> indices;

	Model terrain;
public:
	Terrain();
	Terrain(vec3 startPosition, const char *heightMapPath, string texturePath);
	~Terrain();

	unsigned char* loadHeightMap(const char *path);
	void createTerrain();
	void createNormalMap();
	void triangulate();
	float getPixelColor(vec2 pos);

	void sendToModel();
	void Draw(ShaderCreater shader);

};

#endif // Terrain

#ifndef TERRAIN_H
#define TERRAIN_H

#include "ShaderCreater.h"
#include "Object.h"
#include <vector>
#include "Defines.h"

using namespace glm;
using namespace std;

class Terrain
{
private:
	vec3 terrainPosition;

	float maxHeight;
	float imageScale;
	float terrainSize;

	string texturePath;
	unsigned int heightMapID;
	unsigned char* imageData;
	int imageHeight;	//X-axis
	int imageWidth;		//Z-axis

	float* *heights;

	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec2> uvs;
	vector<unsigned int> indices;


	OBJLoader objLoader;
	Mesh terrain;
public:
	Terrain();
	Terrain(vec3 startPosition, const char *heightMapPath, string texturePath);
	~Terrain();

	void deallocate();

	unsigned char* loadHeightMap(const char *path);
	void createTerrain();
	void triangulate();
	float getPixelColor(vec2 pos);

	void sendToObject();
	void Draw(ShaderCreater shader);

	float getHeightOfTerrain(float worldX, float worldZ);
	float barryCentric(vec3 p1, vec3 p2, vec3 p3, vec2 pos);
	void DrawDepth(ShaderCreater shader);
};

#endif // Terrain

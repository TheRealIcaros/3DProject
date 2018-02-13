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

	string texturePath;
	unsigned int heightMapID;
	unsigned char* imageData;
	int imageHeight;	//X-axis
	int imageWidth;		//Z-axis

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

	unsigned char* loadHeightMap(const char *path);
	void createTerrain();
	void createNormalMap();
	void triangulate();
	float getPixelColor(vec2 pos);

	void sendToObject();
	void Draw(ShaderCreater shader);
};

#endif // Terrain

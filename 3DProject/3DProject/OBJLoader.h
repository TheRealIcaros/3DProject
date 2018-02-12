#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "Defines.h"
#include <vector>
#include "SOIL.h"
#include "ShaderCreater.h"

using namespace std;
using namespace glm;

class OBJLoader
{
private:

public:
	OBJLoader();
	~OBJLoader();

	bool loadOBJ(const char* path, vector<vec3> &vertices, vector<vec2> &uvs, vector<vec3> &normals);
	bool loadMTL();
	unsigned int TextureFromFile(const char* texturePath);
};

#endif

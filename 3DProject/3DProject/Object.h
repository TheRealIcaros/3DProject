#ifndef OBJECT_H
#define OBJECT_H

#include "OBJLoader.h"
#include "Mesh.h"

class Object
{
private:
	OBJLoader objLoader;
	vector<Mesh> meshes;
public:
	Object();
	~Object();

	void loadObject(const char* objPath, const char* texturePath, vec3 startPosition);
	void Draw(ShaderCreater shader);
	unsigned int TextureFromFile(const char* texturePath);
};

#endif // !OBJECT_H

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

	void loadObject(const char* objPath, vec3 startPosition);
	void Draw(ShaderCreater shader);
	void Sort(vec3 cameraPos);
};

#endif // !OBJECT_H

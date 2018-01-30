#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
private:
	/*	Model Data  */
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;
	/*	Funcitons  */
	void loadModel(string path);
	void processNode(aiNode * node, const aiScene *scene);
	Mesh processMesh(aiMesh * mesh, const aiScene *scene);
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
	unsigned int TextureFromFile(const char *path, const string &directory);
public:
	Model();
	Model(char *path);
	~Model();
	void Draw(ShaderCreater shader);
};

#endif // !MODEL_H

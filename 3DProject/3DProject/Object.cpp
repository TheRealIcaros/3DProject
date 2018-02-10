#include "Object.h"

Object::Object()
{

}

Object::~Object()
{
}

void Object::loadObject(const char* objPath, const char* texturePath, vec3 startPosition)
{
	vector<vec3> vertices;
	vector<vec2> uvs;
	vector<vec3> normals;
	this->objLoader.loadOBJ(objPath, vertices, uvs, normals);

	vector<Vertex> vertexes;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex temp;
		temp.Position = vertices[i];
		temp.Normal = normals[i];
		temp.TexCoords = uvs[i];
		vertexes.push_back(temp);
	}

	vector<unsigned int> indices;
	for (unsigned int i = 0; i < vertexes.size() * 3; i++)
	{
		indices.push_back(i);
	}

	vector<Texture> textures;
	Texture texture;
	texture.id = TextureFromFile(texturePath);
	texture.type = "texture_diffuse";
	texture.path = texturePath;
	textures.push_back(texture);

	meshes.push_back(Mesh(vertexes, indices, textures, startPosition));
}

void Object::Draw(ShaderCreater shader)
{
	for (int i = 0; i < this->meshes.size(); i++)
	{
		this->meshes[i].Draw(shader);
	}
}

unsigned int Object::TextureFromFile(const char* texturePath)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGBA;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << texturePath << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

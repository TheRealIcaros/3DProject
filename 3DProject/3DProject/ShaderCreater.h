#ifndef SHADERCREATER_H
#define SHADERCREATER_H

#include <glad\glad.h>
#include "Defines.h"
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>

class ShaderCreater
{
private:
	GLuint programID;

public:
	ShaderCreater();
	~ShaderCreater();

	GLuint getShaderProgramID()const;
	void createShaders(std::string vertexShader, std::string geometryShader, std::string fragmentShader);
	void setFloat(std::string name, int value) const;
};


#endif

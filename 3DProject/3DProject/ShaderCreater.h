#ifndef SHADERCREATER_H
#define SHADERCREATER_H

#include <glad\glad.h>
#include "Defines.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>


class ShaderCreater
{
private:
	GLuint programID;
	GLuint UBO;

public:
	ShaderCreater(std::string vertexShader, std::string geometryShader, std::string fragmentShader, GLuint shaderProgram = 0);
	~ShaderCreater();

	GLuint getShaderProgramID()const;
};


#endif

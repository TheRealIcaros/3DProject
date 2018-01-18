//#include "ShaderClass.h"
//
//ShaderClass::ShaderClass(std::string vertexShader, std::string geometryShader, std::string fragmentShader)
//{
//	//These variables handles error messages
//	GLint success = 0;
//	char infoLog[512];
//
//	//Vertex shader
//	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//	//Open glsl file and put it in a string
//	ifstream shaderFile("VertexShader.glsl");
//	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
//	shaderFile.close();
//	//Make a double pointer (only valid here)
//	const char* shaderTextPtr = shaderText.c_str();
//	//Ask GL to load this
//	glShaderSource(vs, 1, &shaderTextPtr, nullptr);
//
//	//Compile shader
//	glCompileShader(vs);
//
//	//Test if compilation of shader-file went ok
//	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
//	if (success == GL_FALSE)
//	{
//		glGetShaderInfoLog(vs, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//		system("PAUSE");
//		glDeleteShader(vs);
//		exit(-1);
//	}
//
//	//Geometry shader
//	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
//	//Open glsl file and put it in a string
//	shaderFile.open("GeometryShader.glsl");
//	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
//	shaderFile.close();
//	//Make a double pointer (only valid here)
//	shaderTextPtr = shaderText.c_str();
//	//Ask GL to load this
//	glShaderSource(gs, 1, &shaderTextPtr, nullptr);
//
//	//////Compile shader
//	glCompileShader(gs);
//
//	////Test if compilation of shader-file went ok
//	glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
//	if (success == GL_FALSE)
//	{
//		glGetShaderInfoLog(gs, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
//		system("PAUSE");
//		glDeleteShader(gs);
//		exit(-1);
//	}
//
//	//Fragment shader
//	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//	//Open glsl file and put it in a string
//	shaderFile.open("FragmentShader.glsl");
//	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
//	shaderFile.close();
//	//Make a double pointer (only valid here)
//	shaderTextPtr = shaderText.c_str();
//	//Ask GL to load this
//	glShaderSource(fs, 1, &shaderTextPtr, nullptr);
//
//	//Compile shader
//	glCompileShader(fs);
//
//	//Test if compilation of shader-file went ok
//	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
//	if (success == GL_FALSE)
//	{
//		glGetShaderInfoLog(fs, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//		system("PAUSE");
//		glDeleteShader(fs);
//		exit(-1);
//	}
//
//	//Link shader-program (connect vs,(gs) and fs)
//	this->program = glCreateProgram();
//	glAttachShader(this->program, vs);
//	glAttachShader(this->program, gs);
//	glAttachShader(this->program, fs);
//	glLinkProgram(this->program);
//
//	//Create a Uniform Buffer Object(UBO)
//	//Create a buffer name
//	glGenBuffers(1, &this->UBO);
//	//Bind buffer to work further with it
//	glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
//	//Allocate memory for the buffer in the GPU
//	glBufferData(GL_UNIFORM_BUFFER, sizeof(this->valuesFromCPUToGPU), NULL, GL_STATIC_DRAW);
//	//Because we hard-coded "Binding = 3" in the shader we can do this:
//	//Bind Uniform Buffer to binding point 3 (without caring about index of UBO)
//	glBindBufferBase(GL_UNIFORM_BUFFER, 3, UBO);
//	//Good practice , unbind buffer
//	glBindBuffer(GL_UNIFORM_BUFFER, 0);
//
//	//Checks if the linking between the shaders works
//	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
//	if (!success) {
//		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
//		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
//		system("PAUSE");
//		exit(-1);
//	}
//
//	// in any case (compile sucess or not), we only want to keep the 
//	// Program around, not the shaders.
//	glDetachShader(this->program, vs);
//	glDetachShader(this->program, gs);
//	glDetachShader(this->program, fs);
//	glDeleteShader(vs);
//	glDeleteShader(gs);
//	glDeleteShader(fs);
//}
//
//ShaderClass::~ShaderClass()
//{
//}

#include <glad\glad.h>
#include <GLFW/glfw3.h>
#include <crtdbg.h>
#include <stb_image.h> //For loading textures and images

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>

#include "Camera.h"

//3D-math
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//Namespaces
using namespace std;

void initiateGLFW();
GLFWwindow *createWindow();
void gladTest();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void calculateDeltaTime();
void processInput(GLFWwindow *window);
void createShaders();
void setTriangleData();
void Render();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void loadTexture(const char* texturePath, GLuint &textureID);
GLuint loadBMPTexture(const char* texturePath, GLuint &textureID);

//GLuint Variables
GLuint shaderProgram = 0;
GLuint VBO = 0;
GLuint VAO = 0;

//Uniform Buffer Object
GLuint UBO = 0;

//Texture
GLuint textureID;

//My Define Values
#define PI 3.14159265359f
#define ROTATION -1.0f

//My Camera
Camera camera;

//Pitch/Yaw Properties
bool firstMouse = true;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;

glm::mat4 WorldMatrix()
{
	glm::mat4 World;

	return World;
}

glm::mat4 ProjectionMatrix()
{
	float FOV = 0.45f * PI;
	float aspectRatio = 640 / 480;

	glm::mat4 Projection = glm::perspective(FOV, aspectRatio, 0.1f, 20.0f);

	return Projection;
}

//My matrices
glm::mat4 World = WorldMatrix();
glm::mat4 Projection = ProjectionMatrix();

//The struct to be stored in a buffer
struct valuesFromCPUToGPU
{
	glm::mat4 World;
	glm::mat4 View;
	glm::mat4 Projection;
};

//The buffer data I send
valuesFromCPUToGPU myBufferData = { World, camera.getView(), Projection };

//Deltatime Variables
struct Time {
	float deltaTime;
	float lastFrame;
	int frames;
	float duration;
	bool active;
};

Time time
{
	0.0f,	//deltaTime
	0.0f,	//lastFrame
	0,		//frames
	0.0f,	//duration
	false	//active
};

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//Initiates GLFW librarys and configure
	initiateGLFW();

	//Create window
	GLFWwindow* window = createWindow();

	//Test of GLAD
	gladTest();
	
	//Set Viewport
	glViewport(0, 0, 800, 600);

	//Activate resize viewport 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Check if mouse is changed
	//glfwSetCursorPosCallback(window, myControls.mouse_Callback(window, xpos, ypos, lookAtVector));
	glfwSetCursorPosCallback(window, mouse_callback);

	//Create Shaders
	createShaders();

	//Set triangle-data
	setTriangleData();

	//Cursor Disabled/non-visible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Depth testing enabled
	glEnable(GL_DEPTH_TEST);

	//Render Loop
	while (!glfwWindowShouldClose(window))
	{
		//Calculate deltaTime and fps to console
		calculateDeltaTime();

		//Check inputs
		processInput(window);

		//Render
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	//De-allocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void initiateGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		//This sets  requierments of Opengl to version 4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//This sets  requierments of Opengl to version x.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow *createWindow()
{
	GLFWwindow* window = glfwCreateWindow(800, 600, "3D Project", NULL, NULL);
	if (window == NULL)
	{
		cout << "Failes to create GLFW window" << endl;
		glfwTerminate();
		std::exit(-1);
	}
	glfwMakeContextCurrent(window);

	return window;
}

void gladTest()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		glfwTerminate();
		std::exit(-1);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void calculateDeltaTime()
{
	float currentFrame = (float)glfwGetTime();
	time.deltaTime = currentFrame - time.lastFrame;
	time.lastFrame = currentFrame;

	time.frames++;
	time.duration += time.deltaTime;
	if (time.duration >= 1.0f && time.active)
	{
		printf("FPS: %d, dt: %d\n", time.frames, time.deltaTime);
		time.frames = 0;
		time.duration = 0.0f;
	}
}

void createShaders()
{
	//These variables handles error messages
	GLint success = 0;
	char infoLog[512];

	//Vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	//Open glsl file and put it in a string
	ifstream shaderFile("VertexShader.glsl");
	std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	//Make a double pointer (only valid here)
	const char* shaderTextPtr = shaderText.c_str();
	//Ask GL to load this
	glShaderSource(vs, 1, &shaderTextPtr, nullptr);

	//Compile shader
	glCompileShader(vs);

	//Test if compilation of shader-file went ok
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(vs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		system("PAUSE");
		glDeleteShader(vs);
		exit(-1);
	}

	//Geometry shader
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	//Open glsl file and put it in a string
	shaderFile.open("GeometryShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	//Make a double pointer (only valid here)
	shaderTextPtr = shaderText.c_str();
	//Ask GL to load this
	glShaderSource(gs, 1, &shaderTextPtr, nullptr);

	//////Compile shader
	glCompileShader(gs);

	////Test if compilation of shader-file went ok
	glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(gs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		system("PAUSE");
		glDeleteShader(gs);
		exit(-1);
	}

	//Fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	//Open glsl file and put it in a string
	shaderFile.open("FragmentShader.glsl");
	shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	shaderFile.close();
	//Make a double pointer (only valid here)
	shaderTextPtr = shaderText.c_str();
	//Ask GL to load this
	glShaderSource(fs, 1, &shaderTextPtr, nullptr);

	//Compile shader
	glCompileShader(fs);

	//Test if compilation of shader-file went ok
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetShaderInfoLog(fs, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		system("PAUSE");
		glDeleteShader(fs);
		exit(-1);
	}
	
	//Link shader-program (connect vs,(gs) and fs)
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, gs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	//Create a Uniform Buffer Object(UBO)
	//Create a buffer name
	glGenBuffers(1, &UBO);
	//Bind buffer to work further with it
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//Allocate memory for the buffer in the GPU
	glBufferData(GL_UNIFORM_BUFFER, sizeof(valuesFromCPUToGPU), NULL, GL_STATIC_DRAW);
	//Because we hard-coded "Binding = 3" in the shader we can do this:
	//Bind Uniform Buffer to binding point 3 (without caring about index of UBO)
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, UBO);
	//Good practice , unbind buffer
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//Checks if the linking between the shaders works
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		system("PAUSE");
		exit(-1);
	}

	// in any case (compile sucess or not), we only want to keep the 
	// Program around, not the shaders.
	glDetachShader(shaderProgram, vs);
	glDetachShader(shaderProgram, gs);
	glDetachShader(shaderProgram, fs);
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
}

void setTriangleData()
{
	float vertices[] = 
	{ 
		/*-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 

		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 

		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f*/

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f

	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //Generates (1) buffer with VBO id

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Binds an array-buffer with VBO 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Adds the vertices-data to said buffer 


	GLuint vertexPos = glGetAttribLocation(shaderProgram, "vertexPosition");

	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLuint texture = glGetAttribLocation(shaderProgram, "vertex_tex");
	glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//loadTexture("../Textures/durkplat.bmp", textureID);
	loadBMPTexture("../Textures/durkplat.bmp", textureID);
	//loadBMPTexture("../Textures/Crate.bmp", textureID);
}

void processInput(GLFWwindow *window)
{
	//System inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
	{
		if (time.active)
			time.active = false;
		else
		{
			time.active = true;
			time.duration = 0.0f;
			time.frames = 0;
		}
	}

	//View inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * camera.getLookAtVector());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * camera.getLookAtVector() * -1.0f);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(glm::cross(camera.getLookAtVector(), camera.getUpVector())) * -1.0f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(glm::cross(camera.getLookAtVector(), camera.getUpVector())));

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * camera.getUpVector());
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * camera.getUpVector() * -1.0f);
}

void Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	//This clears the depth-buffer
	// use the color to clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgram);
	
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureID);

	glBindVertexArray(VAO);

	//Adds a rotation to the World-matrix, making the object(Veritices spinn)
	myBufferData.World = glm::rotate(myBufferData.World, ROTATION * time.deltaTime, glm::vec3(0, 1, 0));

	//Update View buffer with Camera
	myBufferData.View = camera.getView();

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//Update data (Constant Buffer)
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(valuesFromCPUToGPU), &myBufferData);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; //Reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.mouseMovement(xoffset, yoffset);
}
//
//void loadTexture(const char* texturePath, GLuint &textureID)
//{
//	GLint width, height, nrChannels;	//The width, height and number of color channels
//
//	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
//}

GLuint loadBMPTexture(const char* texturePath, GLuint &textureID)
{
	unsigned char header[54]; //All textures will begin with 54-bytes header
	unsigned int dataPos;	//The position in the file where  the actual data begins
	unsigned int width;
	unsigned int height;
	unsigned int textureSize; // = width * height * 3
	//Actual RGB-data
	unsigned char* RGBdata;

	FILE ** textureFile =  new FILE*(); 
	fopen_s(textureFile, texturePath, "rb");
	if (!textureFile) //If texture didn't load
	{
		cout << "Image could not be opened\n" << endl;
		return 0;
	}

	if (fread(header, 1, 54, *textureFile) != 54)//If header is not 54-bytes
	{
		cout << "Not a correct BMP-file" << endl;
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		cout << "Not a correct BMP-file" << endl;
		return 0;
	}

	dataPos = *(int*)&(header[0x0A]);
	textureSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (textureSize == 0)//If the BMP-file is misformatted
		textureSize = width * height * 3;

	if (dataPos == 0)
		dataPos = 54;

	RGBdata = new unsigned char[textureSize];

	fread(RGBdata, 1, textureSize, *textureFile);

	fclose(*textureFile);

	
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, RGBdata);
	
	//Free the allocated data
	delete[] RGBdata;
	delete[] textureFile;
}
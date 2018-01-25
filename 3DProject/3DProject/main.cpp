#include <glad\glad.h>
#include <crtdbg.h>
#include <stb_image.h> //For loading textures and images
#include <stdio.h>
#include <sstream>

//Own classes
#include "Camera.h"
#include "ShaderCreater.h"

//3D-math
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

void initiateGLFW();
GLFWwindow *createWindow();
void gladTest();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void calculateDeltaTime();
void processInput(GLFWwindow *window);
//void createShaders();
void setTriangleData();
void Render();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void loadTexture(const char* texturePath, GLuint &textureID);
GLuint loadBMPTexture(const char* texturePath, GLuint &textureID);
void createUBO();
void createGbuffer();
void renderQuad();
void renderGeometryPass();
void renderLightingPass();

//Shader
ShaderCreater geometryPass;
ShaderCreater lightingPass;

//GLuint Variables
GLuint VBO = 0;
GLuint VAO = 0;

//Quad
unsigned int quadVAO = 0;
unsigned int quadVBO;

//Uniform Buffer Object
GLuint UBO = 0;

//Texture
GLuint textureID;

//gbuffer
unsigned int gBuffer, gPosition, gNormal, gColorSpec;
unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

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
valuesFromCPUToGPU gpuBufferData = { World, camera.getView(), Projection };

//Deltatime Variables
struct Time
{
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
	geometryPass.createShaders("VertexShader", "GeometryShader", "FragmentShader");
	lightingPass.createShaders("LightingPassVS", "NULL", "LightingPassFS");

	//Create gbuffers
	createGbuffer(); 

	//Create UBO
	createUBO();

	//Set triangle-data
	setTriangleData();

	//Cursor Disabled/non-visible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Depth testing enabled
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

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

//void createShaders()
//{
	////These variables handles error messages
	//GLint success = 0;
	//char infoLog[512];
	//
	////Vertex shader
	//GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	////Open glsl file and put it in a string
	//ifstream shaderFile("VertexShader.glsl");
	//std::string shaderText((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	//shaderFile.close();
	////Make a double pointer (only valid here)
	//const char* shaderTextPtr = shaderText.c_str();
	////Ask GL to load this
	//glShaderSource(vs, 1, &shaderTextPtr, nullptr);
	//
	////Compile shader
	//glCompileShader(vs);
	//
	////Test if compilation of shader-file went ok
	//glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	//if (success == GL_FALSE)
	//{
	//	glGetShaderInfoLog(vs, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	//	system("PAUSE");
	//	glDeleteShader(vs);
	//	exit(-1);
	//}
	//
	////Geometry shader
	//GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	////Open glsl file and put it in a string
	//shaderFile.open("GeometryShader.glsl");
	//shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	//shaderFile.close();
	////Make a double pointer (only valid here)
	//shaderTextPtr = shaderText.c_str();
	////Ask GL to load this
	//glShaderSource(gs, 1, &shaderTextPtr, nullptr);
	//
	////////Compile shader
	//glCompileShader(gs);
	//
	//////Test if compilation of shader-file went ok
	//glGetShaderiv(gs, GL_COMPILE_STATUS, &success);
	//if (success == GL_FALSE)
	//{
	//	glGetShaderInfoLog(gs, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	//	system("PAUSE");
	//	glDeleteShader(gs);
	//	exit(-1);
	//}
	//
	////Fragment shader
	//GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	////Open glsl file and put it in a string
	//shaderFile.open("FragmentShader.glsl");
	//shaderText.assign((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
	//shaderFile.close();
	////Make a double pointer (only valid here)
	//shaderTextPtr = shaderText.c_str();
	////Ask GL to load this
	//glShaderSource(fs, 1, &shaderTextPtr, nullptr);
	//
	////Compile shader
	//glCompileShader(fs);
	//
	////Test if compilation of shader-file went ok
	//glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	//if (success == GL_FALSE)
	//{
	//	glGetShaderInfoLog(fs, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	//	system("PAUSE");
	//	glDeleteShader(fs);
	//	exit(-1);
	//}
	//
	////Link shader-program (connect vs,(gs) and fs)
	//shaderProgram = glCreateProgram();
	//glAttachShader(shaderProgram, vs);
	//glAttachShader(shaderProgram, gs);
	//glAttachShader(shaderProgram, fs);
	//glLinkProgram(shaderProgram);
	//
	////Create a Uniform Buffer Object(UBO)
	////Create a buffer name
	//glGenBuffers(1, &UBO);
	////Bind buffer to work further with it
	//glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	////Allocate memory for the buffer in the GPU
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(valuesFromCPUToGPU), NULL, GL_STATIC_DRAW);
	////Because we hard-coded "Binding = 3" in the shader we can do this:
	////Bind Uniform Buffer to binding point 3 (without caring about index of UBO)
	//glBindBufferBase(GL_UNIFORM_BUFFER, 3, UBO);
	////Good practice , unbind buffer
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//
	////Checks if the linking between the shaders works
	//glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	//if (!success) {
	//	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	//	std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
	//	system("PAUSE");
	//	exit(-1);
	//}
	//
	//// in any case (compile sucess or not), we only want to keep the 
	//// Program around, not the shaders.
	//glDetachShader(shaderProgram, vs);
	//glDetachShader(shaderProgram, gs);
	//glDetachShader(shaderProgram, fs);
	//glDeleteShader(vs);
	//glDeleteShader(gs);
	//glDeleteShader(fs);
//}

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

		 /*-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
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
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f*/

		//Back Face
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		//Front Face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		//Left Face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		//Right Face
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		//Bottom Face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		//Top Face
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //Generates (1) buffer with VBO id

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Binds an array-buffer with VBO 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Adds the vertices-data to said buffer 


	GLuint vertexPos = glGetAttribLocation(geometryPass.getShaderProgramID(), "vertexPosition");

	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLuint texture = glGetAttribLocation(geometryPass.getShaderProgramID(), "vertex_tex");
	glVertexAttribPointer(texture, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//loadTexture("../Textures/durkplat.bmp", textureID);
	loadBMPTexture("../Textures/durkplat.bmp", textureID);
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
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(camera.getLookAtVector()));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(camera.getLookAtVector()) * -1.0f);
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
	//Background Color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//Update Inputs
	gpuBufferData.View = camera.getView();

	//1. Geometry Pass
	renderGeometryPass();

	//2. Lighting Pass
	renderLightingPass();
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

void createUBO()
{
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
}

void createGbuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	//position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	//normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	//color + specular color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	//tell OPENGL which color vi ska använda (av denna framebuffer) for rendering på svenska
	//TOP OF THE KOD
	glDrawBuffers(3, attachments);
	//add djupbufé 

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
}

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void renderGeometryPass()
{
	//Use GeometryPass Shader Program
	glUseProgram(geometryPass.getShaderProgramID());
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind BoxTexture
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(geometryPass.getShaderProgramID(), "texSampler"), 0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glUniformMatrix4fv(glGetUniformLocation(geometryPass.getShaderProgramID(), "View"), 1, GL_FALSE, &gpuBufferData.View[0][0]);

	//Bind UBO for sending GPU data to GeometryPass Program
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(valuesFromCPUToGPU), &gpuBufferData);

	//Bind and Render Vertices
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderLightingPass()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use LightingPass Shader Program
	glUseProgram(lightingPass.getShaderProgramID());

	//	Bind all gBufferTextures
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gPosition"), 0);
	glBindTexture(GL_TEXTURE_2D, gPosition);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gNormal"), 1);
	glBindTexture(GL_TEXTURE_2D, gNormal);

	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gColorSpec"), 2);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);

	//	TODO:(Fix multiple lights and send it to LightingPassFS)
	glUniform3f(glGetUniformLocation(lightingPass.getShaderProgramID(), "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

	//Render To Quad
	renderQuad();
}
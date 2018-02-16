#include <glad\glad.h>
#include <crtdbg.h>
//#include <stb_image.h> //For loading textures and images
#include <stdio.h>

//Own classes
#include "Camera.h"
#include "ShaderCreater.h"
#include "Model.h"
#include "Defines.h"

//3D-math
//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include <gtc/type_ptr.hpp>

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
//GLuint loadBMPTexture(const char* texturePath, GLuint &textureID);
void createUBO();
void createGbuffer();
void createGaussBuffer();
void createLightBuffer();
void renderQuad();
void renderGeometryPass();
void renderLightingPass();
void renderBlurPass();
void renderMergePass();

//Shader
ShaderCreater geometryPass;
ShaderCreater lightingPass;
ShaderCreater gaussPass;
ShaderCreater mergePass;

//Lights
struct Light
{
	Light(glm::vec3 pos, glm::vec3 color)
	{
		lightPos = pos;
		lightColor = color;
	}

	glm::vec3 lightPos;
	glm::vec3 lightColor;
};

vector<Light> lights;

//Model
vector<Model> models;

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
GLuint textureID2;


//gbuffer
unsigned int gBuffer, gPosition, gNormal, gColorSpec;
//lightbuffer
unsigned int lBuffer, lColor, lGlow;
// Glow textures
unsigned int pingPongFBO[2], pingPongBuff[2];

bool bloomKey = false;
bool glowKey = false;
bool intensityKey = false;

//My Camera
Camera camera = Camera(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 0.0, -1.0));

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
	geometryPass.createShaders("GeometryPassVS", "NULL", "GeometryPassFS");
	lightingPass.createShaders("LightingPassVS", "NULL", "LightingPassFS");
	gaussPass.createShaders("GaussVS", "NULL", "GaussFS");
	mergePass.createShaders("MergeVS", "NULL", "MergeFS");

	//Create buffers
	createGbuffer();
	createLightBuffer();
	createGaussBuffer();
	
	//Create UBO
	createUBO();

	//Set triangle-data
	//setTriangleData();

	//Add lights
	lights.push_back(Light(glm::vec3(0.0, 0.0, -5.0), glm::vec3(0.0, 0.0, 1.0)));
	lights.push_back(Light(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0.0, 1.0, 0.0)));
	lights.push_back(Light(glm::vec3(5.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));

	lights.push_back(Light(glm::vec3(0.0, 5.0, 0.0), glm::vec3(1.0, 1.0, 1.0)));
	
	//Add Models
	models.push_back(Model("../Models/HDMonkey/HDMonkey.obj", glm::vec3(2.0, 0.0, 0.0)));
	models.push_back(Model("../Models/Box/Box.obj", glm::vec3(-2.0, 0.0, 0.0)));

	//Cursor Disabled/non-visible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Depth testing enabled
	glEnable(GL_DEPTH_TEST);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

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

void setTriangleData()
{
	float vertices[] = 
	{ 
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
	//loadBMPTexture("../Textures/durkplat.bmp", textureID);
	//loadBMPTexture("../Textures/ball.bmp", textureID2);
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
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)	
		if (!bloomKey)
		{
			bloomKey = !bloomKey;
		}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		if (!glowKey)
		{
			glowKey = !glowKey;
		}
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		if (!intensityKey)
		{
			intensityKey = !intensityKey;
		}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		bloomKey = false;
		glowKey = false;
		intensityKey = false;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
	{
		bloomKey = !false;
		glowKey = !false;
		intensityKey = !false;
	}

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

	//3. Blurr Pass
	renderBlurPass();

	//4. Merge Pass
	renderMergePass();
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

	//tell OPENGL which color we are going to use (from this framebuffer) for rendering
	//TOP OF THE CODE
	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	//add depthbuffer 

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
}

void createGaussBuffer()
{
	// Blurring that will later become glow relevant in merge
	glGenFramebuffers(2, pingPongFBO);
	glGenTextures(2, pingPongBuff);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongBuff[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongBuff[i], 0);
	}
}

void createLightBuffer()
{
	glGenFramebuffers(1, &lBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);


	//position color buffer
	glGenTextures(1, &lColor);
	glBindTexture(GL_TEXTURE_2D, lColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lColor, 0);

	//normal color buffer
	glGenTextures(1, &lGlow);
	glBindTexture(GL_TEXTURE_2D, lGlow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lGlow, 0);

	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
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

	glUniformMatrix4fv(glGetUniformLocation(geometryPass.getShaderProgramID(), "View"), 1, GL_FALSE, &gpuBufferData.View[0][0]);

	//Bind UBO for sending GPU data to GeometryPass Program
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(valuesFromCPUToGPU), &gpuBufferData);
		
	for (int i = 0; i < models.size(); i++)
	{
		models[i].Draw(geometryPass);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderLightingPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, lBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use LightingPass Shader Program
	glUseProgram(lightingPass.getShaderProgramID());

	//	Bind all gBufferTextures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gPosition"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gNormal"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gColorSpec"), 2);

	//	TODO:(Fix multiple lights and send it to LightingPassFS)
	GLuint lightPos = glGetUniformLocation(lightingPass.getShaderProgramID(), "nrOfLights");
	glUniform1i(lightPos, lights.size());
	for (int i = 0; i < lights.size(); i++)
	{
		string lightPos = "lights[" + std::to_string(i) + "].Position";
		string lightColor = "lights[" + std::to_string(i) + "].Color";

		glUniform3fv(glGetUniformLocation(lightingPass.getShaderProgramID(), lightPos.c_str()), 1, &lights[i].lightPos[0]);
		glUniform3fv(glGetUniformLocation(lightingPass.getShaderProgramID(), lightColor.c_str()), 1, &lights[i].lightColor[0]);
	}

	glUniform3f(glGetUniformLocation(lightingPass.getShaderProgramID(), "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "glowKey"), glowKey);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "intensityKey"), intensityKey);


	//Render To Quad
	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//blur pass
void renderBlurPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[0]);
	glUseProgram(gaussPass.getShaderProgramID());
	glUniform1i(glGetUniformLocation(gaussPass.getShaderProgramID(), "input"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lGlow);
	
	glUniform1i(glGetUniformLocation(gaussPass.getShaderProgramID(), "horizontal"), 0);

	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[1]);
	glBindTexture(GL_TEXTURE_2D, pingPongBuff[0]);
	glUniform1i(glGetUniformLocation(gaussPass.getShaderProgramID(), "horizontal"), 1);

	renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}


void renderMergePass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(mergePass.getShaderProgramID());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lColor);
	glUniform1i(glGetUniformLocation(mergePass.getShaderProgramID(), "lColor"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingPongBuff[1]);
	glUniform1i(glGetUniformLocation(mergePass.getShaderProgramID(), "lGlow"), 1);

	glUniform1i(glGetUniformLocation(mergePass.getShaderProgramID(), "bloomKey"), bloomKey);

	renderQuad();
}
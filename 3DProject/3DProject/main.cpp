#include <glad\glad.h>
#include <crtdbg.h>
#include <stdio.h>
#include <gl\GL.h>
#include <matrix.hpp>
//Own classes
#include "Object.h"
#include "Camera.h"
#include "Defines.h"
#include "Model.h"
#include "ShaderCreater.h"
#include "Terrain.h"

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
void Render();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void createUBO();
void createGbuffer();
void createDepthMapFBO();
void renderQuad();
void renderTerrainPass();
void renderGeometryPass();
void renderLightingPass();
void renderShadowMapping();
void renderFrustum();
void frustum();
  
//Shader
ShaderCreater terrainPass;
ShaderCreater geometryPass;
ShaderCreater lightingPass;
ShaderCreater frustumPass;

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


//Shadow Mapping
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024; 
unsigned int depthMapFBO;
unsigned int depthMap;
glm::mat4 lightView, lightProjection; //Matrixes for the shadow mapping
glm::mat4 ligthSpaceTransFormMatrix; //Changes world-space coordinates to light-space coordingates

//Terrain
Terrain terrain;

//Object
Object objects; 

//GLuint Variables
GLuint VBO = 0;
GLuint VAO = 0;

//Quad
unsigned int quadVAO = 0;
unsigned int quadVBO;

//Uniform Buffer Object
GLuint UBO = 0;

//Texture
GLuint bth_tex;
GLuint textureID;
GLuint textureID2;

//gbuffer
unsigned int gBuffer, gPosition, gNormal, gColorSpec, gColorInfo;
unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

//My Camera & camera values
Camera camera;
Camera frustumCamera(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
bool cameraSwaped = false;

//Pitch/Yaw Properties
bool firstMouse = true;
double lastX = WIDTH / 2.0f;
double lastY = HEIGHT / 2.0f;

//Projection Matrix values
float FOV = 0.45f * PI;
float aspectRatio = WIDTH / HEIGHT;
float nearPlane = 0.1f;
float farPlane = 50.0f;

//Frustum values
float halfHeight = tanf(DegreseToRadians * (FOV / 2.f));
float halfWidth = halfHeight * aspectRatio;
float xNearPlane = halfWidth * nearPlane;
float xFarPlane = halfWidth * farPlane;
float yNearPlane = halfHeight * nearPlane;
float yFarPlane = halfHeight * farPlane;

glm::vec4 faces[8] =
{
	/*{ xNearPlane, yNearPlane,  nearPlane,  1.0f},
	{-xNearPlane, yNearPlane,  nearPlane,  1.0f },
	{ xNearPlane, -yNearPlane, nearPlane,  1.0f },
	{-xNearPlane, -yNearPlane, nearPlane,  1.0f },
	//
	{  xFarPlane, yFarPlane, farPlane,  1.0f },
	{ -xFarPlane, yFarPlane, farPlane,  1.0f },
	{  xFarPlane, -yFarPlane, farPlane, 1.0f },
	{ -xFarPlane, -yFarPlane, farPlane, 1.0f }*/


	//NDC near face
	{ 1, 1, -1, 1 },
	{ -1, 1, -1, 1 },
	{ 1, -1, -1, 1 },
	{ -1, -1, -1, 1 },

	//NDC far face
	{ 1, 1, 1, 1 },
	{ -1, 1, 1 , 1 },
	{ 1, -1, 1 , 1 },
	{ -1, -1,1, 1 },
};


glm::mat4 WorldMatrix()
{
	glm::mat4 World;

	return World;
}

glm::mat4 ProjectionMatrix()
{
	//float FOV = 0.45f * PI;
	//float aspectRatio = 640 / 480;
	//float nearPlane = 0.1f;
	//float farPlane = 20.0f;

	glm::mat4 Projection = glm::perspective(FOV, aspectRatio, nearPlane, farPlane);

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
	glViewport(0, 0, WIDTH, HEIGHT);

	//Activate resize viewport 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Check if mouse is changed
	glfwSetCursorPosCallback(window, mouse_callback);

	//Create Shaders
	terrainPass.createShaders("TerrainVS", "TerrainGS", "TerrainFS");
	geometryPass.createShaders("GeometryPassVS", "NULL", "GeometryPassFS");
	lightingPass.createShaders("LightingPassVS", "NULL", "LightingPassFS");
	//geometryPass.createShaders("GeometryPassVS", "NULL", "GeometryPassFS");
	//lightingPass.createShaders("LightingPassVS", "NULL", "LightingPassFS");
	//frustumPass.createShaders("FrustumVS", "FrustumGS", "FrustumFS");

	//frustum();

	//Create Terrain
	terrain = Terrain(vec3(-1, -13.0, -1), "../Models/Terrain/heightMap.bmp", "../Models/Terrain/stoneBrick.png");

	//Object
	objects.loadObject("../Models/HDMonkey/HDMonkey.obj", vec3(26.0, 0.0, 9.0));
	objects.loadObject("../Models/Box/box.obj", glm::vec3(25.0, 0.0, 11.0));

	//Create gbuffers
	createGbuffer(); 

	//Create UBO
	createUBO();

	//Add lights
	lights.push_back(Light(glm::vec3(0.0, 0.0, -5.0), glm::vec3(1.0, 1.0, 1.0)));
	//lights.push_back(Light(glm::vec3(0.0, 0.0, 5.0), glm::vec3(1.0, 1.0, 1.0)));
	//lights.push_back(Light(glm::vec3(5.0, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0)));

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

		//Rendering Deferred + Forward(the outlined frustum) if camera swaped
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	//De-allocate resources
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	terrain.deallocate();
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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "3D Project", NULL, NULL);
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

void processInput(GLFWwindow *window)
{
	//System inputs
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
	{
		time.active = !time.active;
		time.duration = 0.0f;
		time.frames = 0;
	}

	//new View inputs for walking on terrain
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//vec3 lookAt = camera.getLookAtVector();
		//lookAt.y = 0;
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(camera.getLookAtVector()));						//"Normal"-Camera
		frustumCamera.moveCameraPosition((frustumCamera.getSpeed() * time.deltaTime) * frustumCamera.getUpVector());	//Frustum-Camera

		float height = terrain.getHeightOfTerrain(camera.getPosition().x, camera.getPosition().z);	//Collect info about terrain height
		camera.setHeight(height + 1);	//Place camera 1 unit over the terrain
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//vec3 lookAt = camera.getLookAtVector();
		//lookAt.y = 0;
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(camera.getLookAtVector()) * -1.0f);
		frustumCamera.moveCameraPosition((frustumCamera.getSpeed() * time.deltaTime) * frustumCamera.getUpVector() * -1.0f);

		float height = terrain.getHeightOfTerrain(camera.getPosition().x, camera.getPosition().z);	//Collect info about terrain height
		camera.setHeight(height + 1);	//Place camera 1 unit over the terrain
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(glm::cross(camera.getLookAtVector(), camera.getUpVector())) * -1.0f);
		frustumCamera.moveCameraPosition((frustumCamera.getSpeed() * time.deltaTime) * glm::normalize(glm::cross(frustumCamera.getLookAtVector(), frustumCamera.getUpVector())) * -1.0f);

		float height = terrain.getHeightOfTerrain(camera.getPosition().x, camera.getPosition().z);	//Collect info about terrain height
		camera.setHeight(height + 1);	//Place camera 1 unit over the terrain
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * glm::normalize(glm::cross(camera.getLookAtVector(), camera.getUpVector())));
		frustumCamera.moveCameraPosition((frustumCamera.getSpeed() * time.deltaTime) * glm::normalize(glm::cross(frustumCamera.getLookAtVector(), frustumCamera.getUpVector())));

		float height = terrain.getHeightOfTerrain(camera.getPosition().x, camera.getPosition().z); //Collect info about terrain height
		camera.setHeight(height + 1); //Place camera 1 unit over the terrain
	}
		
	//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && cameraSwaped != true)
	//	camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * camera.getUpVector());
	//if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && cameraSwaped != true)
	//	camera.moveCameraPosition((camera.getSpeed() * time.deltaTime) * camera.getUpVector() * -1.0f);


	//Change between the two cameras
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	{
		if (cameraSwaped == false)
		{
			cameraSwaped = true;	// Frustum camera
			frustumCamera.setCameraPosition(camera.getPosition().x, (camera.getPosition().y + 10.0f), camera.getPosition().z);
		}
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{

		if (cameraSwaped == true)// "Original" camera
			cameraSwaped = false;
	}
}

void Render()
{
	//Background Color
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	//Update Inputs
	if (cameraSwaped == false)
		gpuBufferData.View = camera.getView();
	else
	{
		gpuBufferData.View = frustumCamera.getView();
		
		//Rendering forward
		//renderFrustum();
	}

	//0.5 Terrain Pass
	renderTerrainPass();
	
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

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; //Reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	if (cameraSwaped == true)	// If we looking throu the top-down camera,
		yoffset = 0.0f;		   //we can't move the view-frustum of the other camera up and down
	
	
	camera.mouseMovement((float)xoffset, (float)yoffset);
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

	//color buffer
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

	//Ambient, Diffuse, Specular och Shininess color buffer
	glGenTextures(1, &gColorInfo);
	glBindTexture(GL_TEXTURE_2D, gColorInfo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gColorInfo, 0);

	//tell OPENGL which color vi ska använda (av denna framebuffer) for rendering på svenska
	//TOP OF THE KOD
	glDrawBuffers(4, attachments);
	//add djupbufé 

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
}

void createDepthMapFBO()
{
	glGenFramebuffers(1, &depthMapFBO);
	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void renderTerrainPass()
{
	//Use TerrainPass Shader Program
	glUseProgram(terrainPass.getShaderProgramID());
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind UBO for sending GPU data to TerrainPass Program
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(valuesFromCPUToGPU), &gpuBufferData);

	terrain.Draw(terrainPass);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderGeometryPass()
{
	//Use GeometryPass Shader Program
	glUseProgram(geometryPass.getShaderProgramID());
	/*glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

	//Bind UBO for sending GPU data to GeometryPass Program
	/*glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(valuesFromCPUToGPU), &gpuBufferData);*/

	objects.Draw(geometryPass);

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

	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "gColorInfo"), 3);
	glBindTexture(GL_TEXTURE_2D, gColorInfo);

	//Lights
	glUniform1i(glGetUniformLocation(lightingPass.getShaderProgramID(), "nrOfLights"), lights.size());
	for (unsigned int i = 0; i < lights.size(); i++)
	{
		string lightPos = "lights[" + std::to_string(i) + "].Position";
		string lightColor = "lights[" + std::to_string(i) + "].Color";

		glUniform3fv(glGetUniformLocation(lightingPass.getShaderProgramID(), lightPos.c_str()), 1, &lights[i].lightPos[0]);
		glUniform3fv(glGetUniformLocation(lightingPass.getShaderProgramID(), lightColor.c_str()), 1, &lights[i].lightColor[0]);
	}
	if(cameraSwaped == false)
		glUniform3f(glGetUniformLocation(lightingPass.getShaderProgramID(), "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
	else
		glUniform3f(glGetUniformLocation(lightingPass.getShaderProgramID(), "viewPos"), frustumCamera.getPosition().x, frustumCamera.getPosition().y, frustumCamera.getPosition().z);

	//Render To Quad
	renderQuad();
}

void renderShadowMapping()
{
	//1. First renderpass in shadow mapping
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
	lightView = glm::lookAt(lights[0].lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ligthSpaceTransFormMatrix = lightProjection * lightView;
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT); //This sets the viewport to the shadow-mappings resolution
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT); //This clears the depth buffer

	

	//2. Second renderpass in shadow mapping
	glViewport(0, 0, WIDTH, HEIGHT); // Sets the viewport to the resolution of the application window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This clears both the color buffer and the depth buffer
	glBindTexture(GL_TEXTURE_2D, depthMap); 
}

void renderFrustum()
{
	//Set backgroundcolor
	glClearColor(0.50f, 0.50f, 0.50f, 0.50f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(frustumPass.getShaderProgramID());

	glBindVertexArray(VAO);

	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(valuesFromCPUToGPU), &gpuBufferData);

	glDrawArrays(GL_LINE_STRIP, 0, 24);
	//glDrawArrays(GL_TRIANGLES, 0, 8);
}

void frustum()
{

	glm::vec3 verticies[8];
	for (int i=0; i < 8; i++)
	{
		glm::vec4 ff = glm::inverse(gpuBufferData.Projection * camera.getView()) * faces[i];
		

		verticies[i].x = ff.x / ff.w;
		verticies[i].y = ff.y / ff.w;
		verticies[i].z = ff.z / ff.w;
	}

	// Vertex Array Object (VAO) 
	glGenVertexArrays(1, &VAO);
	// create a vertex buffer object (VBO) id
	glGenBuffers(1, &VBO);

	// bind == enable
	glBindVertexArray(VAO);

	// Bind the buffer ID as an ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// This "could" imply copying to the GPU immediately, depending on what the driver wants to do...
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	GLuint vertexPos = glGetAttribLocation(frustumPass.getShaderProgramID(), "vertex_position");
	if (vertexPos == -1)
	{
		OutputDebugStringA("Error, cannot find 'vertex_position' attribute in Vertex shader\n");
		return;
	}

	glVertexAttribPointer(vertexPos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(0));
	// this activates the first and second attributes of this VAO
	glEnableVertexAttribArray(0);	

	// find out location of input vertex_position in the Vertex Shader 
	
	// specify that: the vertex attribute at location "vertexPos", of 3 elements of type FLOAT, 
	// not normalized, with STRIDE != 0, starts at offset 0 of the gVertexBuffer (it is the last bound!)

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
}
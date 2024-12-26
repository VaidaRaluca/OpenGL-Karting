// ViewOBJModel.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <Windows.h>
#include <locale>
#include <codecvt>

#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 

#include <GL/glew.h>

#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
Camera* pCamera = nullptr;
glm::vec3 carPosition(1.5f, 0.0f, 0.0f); // Initial car position


void Cleanup()
{
	delete pCamera;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float carSpeed = 0.1f;
	float rotationSpeed = 0.5f;
	glm::vec3 cameraOffset(0.0f, 0.5f, 0.58f);  // Camera offset from car
	float carRotationAngle = 0.0f;

	// Handle ESC key to close the window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Move the car forward and update the camera's offset accordingly
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		carPosition += glm::vec3(0.0f, 0.0f, -carSpeed);
		cameraOffset.z -= carSpeed;  // Move camera with the car
	}

	// Move the car backward and update the camera's offset accordingly
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		carPosition += glm::vec3(0.0f, 0.0f, carSpeed);
		cameraOffset.z += carSpeed;  // Move camera with the car
	}

	// Move the car left and update the camera's offset accordingly
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		carPosition += glm::vec3(-carSpeed, 0.0f, 0.0f);
		cameraOffset.x -= carSpeed;  // Move camera with the car
		carRotationAngle += rotationSpeed;
	}

	// Move the car right and update the camera's offset accordingly
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		carPosition += glm::vec3(carSpeed, 0.0f, 0.0f);
		cameraOffset.x += carSpeed;  // Move camera with the car
		carRotationAngle -= rotationSpeed;
	}

	// Reset camera when 'R' is pressed
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);
	}

	// Calculate the camera's rotation based on the car's rotation angle
	glm::mat4 cameraRotation = glm::rotate(glm::mat4(1.0f), glm::radians(carRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	cameraOffset = glm::vec3(cameraRotation * glm::vec4(cameraOffset, 0.0f));  // Rotate camera's offset

	// Update the camera position to follow the car
	glm::vec3 updatedCameraPosition = carPosition + cameraOffset;
	pCamera->SetPosition(updatedCameraPosition);  // Update the camera's position

	// Print current camera and car positions for debugging
	glm::vec3 cameraPos = pCamera->GetPosition();
	std::cout << "Camera Position: ("
		<< cameraPos.x << ", "
		<< cameraPos.y << ", "
		<< cameraPos.z << ")" << std::endl;

	std::cout << "Car Position: ("
		<< carPosition.x << ", "
		<< carPosition.y << ", "
		<< carPosition.z << ")" << std::endl;

}


int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Karting", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Set up vertex data and buffers for the cube and light sources
	float vertices[] = {
		// Coordinates and normals for each vertex
		// Cube vertices (with normals for Phong lighting)
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		// Cube front face, back face, left, right, bottom and top...
		// (Add all faces here, just as you have in the provided code)
	};

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// Set up the vertex attributes for the cube (positions and normals)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set up the light source's VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Create the camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(1.15f, 0.5f, 0.58f));


	glm::vec3 lightPos(0.0f, 2.0f, 1000.0f);
	glm::vec3 cubePos(0.0f, 5.0f, 1.0f);

	// Get the current executable path for shader loading
	wchar_t buffer[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, buffer);
	std::wstring executablePath(buffer);
	std::wstring wscurrentPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	std::string currentPath = converter.to_bytes(wscurrentPath);

	// Load shaders
	Shader lightingShader((currentPath + "\\Shaders\\PhongLight.vs").c_str(), (currentPath + "\\Shaders\\PhongLight.fs").c_str());
	Shader lightingWithTextureShader((currentPath + "\\Shaders\\PhongLightWithTexture.vs").c_str(), (currentPath + "\\Shaders\\PhongLightWithTexture.fs").c_str());
	Shader lampShader((currentPath + "\\Shaders\\Lamp.vs").c_str(), (currentPath + "\\Shaders\\Lamp.fs").c_str());

	// Load models
	std::string carObjFileName = (currentPath + "\\Models\\Car\\car.obj");
	Model carObjModel(carObjFileName, false);
	std::string grassObjFileName = (currentPath + "\\Models\\Grass\\Grass.obj");
	Model grassObjModel(grassObjFileName, false);
	std::string roadObjFileName = (currentPath + "\\Models\\road\\road.obj");
	Model roadObjModel(roadObjFileName, false);

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Time per frame
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Clear color and depth buffers
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update light and object positions
		lightPos.y = 2.5f * cos(glfwGetTime());
		lightPos.z = 2.5f * sin(glfwGetTime());

		cubePos.x = 10 * sin(glfwGetTime());
		cubePos.z = 10 * cos(glfwGetTime());


		// Use lighting shader and set uniform variables
		lightingShader.use();
		lightingShader.SetVec3("objectColor", 1.0f, 1.0f, 0.31f);
		lightingShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.SetVec3("lightPos", lightPos);
		lightingShader.SetVec3("viewPos", pCamera->GetPosition());
		lightingShader.setMat4("projection", pCamera->GetProjectionMatrix());
		lightingShader.setMat4("view", pCamera->GetViewMatrix());

		// Render objects with lighting
		lightingWithTextureShader.use();
		lightingWithTextureShader.SetVec3("objectColor", 1.0f, 0.0f, 0.0f);
		lightingWithTextureShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingWithTextureShader.SetVec3("lightPos", lightPos);
		lightingWithTextureShader.SetVec3("viewPos", pCamera->GetPosition());
		lightingWithTextureShader.setInt("texture_diffuse1", 0);
		lightingWithTextureShader.setMat4("projection", pCamera->GetProjectionMatrix());
		lightingWithTextureShader.setMat4("view", pCamera->GetViewMatrix());


		// Render grass model
		glm::mat4 grassModel = glm::scale(glm::mat4(1.0f), glm::vec3(1000.f, 1.0f, 1000.0f));
		lightingWithTextureShader.setMat4("model", grassModel);
		grassObjModel.Draw(lightingWithTextureShader);

		// Render road model
		glm::mat4 roadModel = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.9f, 0.72f));
		roadModel = glm::rotate(roadModel, glm::radians(130.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		lightingWithTextureShader.setMat4("model", roadModel);
		roadObjModel.Draw(lightingWithTextureShader);

		// Render car model
		glm::mat4 carModel = glm::translate(glm::mat4(1.0f), carPosition);
		carModel = glm::scale(carModel, glm::vec3(2.5f)); // Scaling the car
		carModel = glm::rotate(carModel, glm::radians(150.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate the car 180 degrees around the Y-axis
		/*carModel = glm::translate(carModel, glm::vec3(-0.5f, 0.0f, 0.0f));*/ // Adjust y value to raise the car above the road
		lightingWithTextureShader.setMat4("model", carModel);
		carObjModel.Draw(lightingWithTextureShader);


		// Draw the lamp (light source)
		lampShader.use();
		lampShader.setMat4("projection", pCamera->GetProjectionMatrix());
		lampShader.setMat4("view", pCamera->GetViewMatrix());
		glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPos);
		lightModel = glm::scale(lightModel, glm::vec3(0.05f)); // smaller cube
		lampShader.setMat4("model", lightModel);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	Cleanup();
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// Terminate GLFW
	glfwTerminate();
	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	pCamera->Reshape(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
	pCamera->ProcessMouseScroll((float)yOffset);
}

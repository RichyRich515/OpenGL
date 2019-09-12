// Main.cpp
// Entrypoint for program
// 2019-09-04

#include "GLCommon.h"

#include <stdlib.h>
#include <stdio.h>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cMesh.hpp"
#include "cModelLoader.hpp";
#include "cVAOManager.hpp";
#include "cShaderManager.hpp"
#include "cGameObject.hpp";

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraEye = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraEye - cameraFront);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

float CAMERA_SPEED = 3.0f;

float cameraSensitivity = 0.1f;

// Camera movement
bool fast = false, mF = false, mB = false, mL = false, mR = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_W)
		if (action == GLFW_PRESS)
			mF = true;
		else if (action == GLFW_RELEASE)
			mF = false;

	if (key == GLFW_KEY_S)
		if (action == GLFW_PRESS)
			mB = true;
		else if (action == GLFW_RELEASE)
			mB = false;

	if (key == GLFW_KEY_A)
		if (action == GLFW_PRESS)
			mL = true;
		else if (action == GLFW_RELEASE)
			mL = false;

	if (key == GLFW_KEY_D)
		if (action == GLFW_PRESS)
			mR = true;
		else if (action == GLFW_RELEASE)
			mR = false;

	if (key == GLFW_KEY_LEFT_SHIFT)
		if (action == GLFW_PRESS)
			fast = true;
		else if (action == GLFW_RELEASE)
			fast = false;
}

int main()
{
	GLFWwindow* window;
	GLint mvp_location;
	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	
	// Set minimum versions of OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(1920, 1080, "OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor and lock to window
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1); // Same idea as vsync, setting this to 0 would result in unlocked framerate and potentially cause screen tearing
	
	cModelLoader* pModelLoader = new cModelLoader();
	cMesh* mesh = new cMesh();
	cVAOManager* pVAOManager = new cVAOManager();
	cShaderManager* pShaderManager = new cShaderManager();

	if (!pModelLoader->loadModel("assets/models/bun_zipper_res4_xyz.ply", mesh))
	{
		std::cerr << "Failed to load Model" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	std::string str;
	cShaderManager::cShader vertexShader01;
	vertexShader01.fileName = "assets/shaders/vertexShader01.glsl";
	vertexShader01.bSourceIsMultiLine = true;

	cShaderManager::cShader fragmentShader01;
	fragmentShader01.fileName = "assets/shaders/fragmentShader01.glsl";
	fragmentShader01.bSourceIsMultiLine = true;

	if (!pShaderManager->createProgramFromFile("shader01", vertexShader01, fragmentShader01))
	{
		std::cerr << "Failed to create shader program: " << pShaderManager->getLastError() << std::endl;
	}

	GLuint program = pShaderManager->getIDFromFriendlyName("shader01");
	glUseProgram(program);
	mvp_location = glGetUniformLocation(program, "MVP");

	// Default
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Also see GL_POINT

	sModelDrawInfo drawInfo;
	if (!pVAOManager->LoadModelIntoVAO("bunny", mesh, drawInfo, program))
	{
		std::cerr << "Failed to load model to GPU" << std::endl;
		exit(EXIT_FAILURE);
	}

	cGameObject bunny1;
	bunny1.meshName = "bunny";
	bunny1.position = glm::vec3(0.0f, -0.5f, 0.0f);
	bunny1.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	bunny1.scale = 4.0f;
	bunny1.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	std::vector<cGameObject> vecGameObjects;
	vecGameObjects.push_back(bunny1);

	float ratio;
	int width, height;
	glm::mat4 m, v, p, mvp;
 	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);

	// timing
	float totalTime;
	float lastTime = 0;
	float dt;

	double cursorX = 0, cursorY = 0, lastcursorX = 0, lastcursorY = 0;

	glfwGetCursorPos(window, &cursorX, &cursorY);
	lastcursorX = cursorX;
	lastcursorY = cursorY;

	float pitch = 0.0f; // X axis rotation ( up, down)
	float yaw = -90.0f; // Y axis rotation (left, right)

	while (!glfwWindowShouldClose(window))
	{
		totalTime = glfwGetTime();
		dt = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);

		// Camera Movement
		glfwGetCursorPos(window, &cursorX, &cursorY);

		yaw += (cursorX - lastcursorX) * cameraSensitivity;
		pitch += (lastcursorY - cursorY) * cameraSensitivity;
		lastcursorX = cursorX;
		lastcursorY = cursorY;

		if (pitch > 89.9f)
			pitch = 89.9f;
		else if (pitch < -89.9f)
			pitch = -89.9f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);

		float cameraSpeed = CAMERA_SPEED * (fast ? 2 : 1);

		cameraEye += (mF - mB) * cameraSpeed * dt * cameraFront;
		cameraEye += (mR - mL) * cameraSpeed * dt * glm::normalize(glm::cross(cameraFront, cameraUp));

		std::ostringstream windowTitle;
		windowTitle << std::fixed << std::setprecision(2) << "Eye: {" << cameraEye.x << ", " << cameraEye.y << ", " << cameraEye.z << "} "
					<< "Front: {" << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << "} "
					<< std::setprecision(0) << "FPS: " << 1 / dt;
		glfwSetWindowTitle(window, windowTitle.str().c_str());

		for (unsigned i = 0; i != vecGameObjects.size(); ++i)
		{
			m = glm::mat4(1.0f);

			glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), vecGameObjects[i].rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), vecGameObjects[i].rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), vecGameObjects[i].rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(vecGameObjects[i].scale, vecGameObjects[i].scale, vecGameObjects[i].scale));
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), vecGameObjects[i].position);

			m *= rotationX * rotationY * rotationZ * translation * scale;

			// FOV, aspect ratio, near clip, far clip
			p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);

			v = glm::mat4(1.0f);
			v = glm::lookAt(cameraEye, cameraEye + cameraFront, cameraUp);

			mvp = p * v * m; // Dont change this order.

			glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

			sModelDrawInfo drawInfo;
			if (pVAOManager->FindDrawInfoByModelName("bunny", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
		glfwSwapBuffers(window); // Draws to screen
		glfwPollEvents(); // Keyboard/Mouse input, etc.
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	// Delete everything
	delete mesh;
	delete pModelLoader;
	delete pVAOManager;
	exit(EXIT_SUCCESS);
}

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
#include "cModelLoader.hpp"
#include "cVAOManager.hpp"
#include "cShaderManager.hpp"
#include "cGameObject.hpp"
#include "cLight.hpp"
#include "Physics.hpp"


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraEye = glm::vec3(0.0f, 8.0f, 10.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraEye - cameraFront);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

float CAMERA_SPEED = 3.0f;

float cameraSensitivity = 0.1f;

// Camera movement
bool shift_pressed = false, mF = false, mB = false, mL = false, mR = false, mU = false, mD = false, rPress = false, fPress = false;

std::vector<cGameObject*> vecGameObjects;
int selectedObject = 0;

std::vector<cLight*> vecLights;
int selectedLight = 1;

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

	if (key == GLFW_KEY_SPACE)
		if (action == GLFW_PRESS)
			mU = true;
		else if (action == GLFW_RELEASE)
			mU = false;

	if (key == GLFW_KEY_C)
		if (action == GLFW_PRESS)
			mD = true;
		else if (action == GLFW_RELEASE)
			mD = false;

	if (key == GLFW_KEY_R)
		if (action == GLFW_PRESS)
			rPress = true;
		else if (action == GLFW_RELEASE)
			rPress = false;

	if (key == GLFW_KEY_F)
		if (action == GLFW_PRESS)
			fPress = true;
		else if (action == GLFW_RELEASE)
			fPress = false;

	if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
	{
		++selectedObject;
		if (selectedObject >= vecGameObjects.size())
			selectedObject = 0;
	}
	else if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
	{
		--selectedObject;
		if (selectedObject < 0)
			selectedObject = vecGameObjects.size() - 1;
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		vecGameObjects[selectedObject]->visible = !vecGameObjects[selectedObject]->visible;
	}

	if (key == GLFW_KEY_LEFT_SHIFT)
		if (action == GLFW_PRESS)
			shift_pressed = true;
		else if (action == GLFW_RELEASE)
			shift_pressed = false;
}

int main()
{
	GLFWwindow* window;

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
	cVAOManager* pVAOManager = new cVAOManager();
	cShaderManager* pShaderManager = new cShaderManager();

	cMesh* bunnymesh = new cMesh();
	//cMesh* piratemesh = new cMesh();
	cMesh* spheremesh = new cMesh();
	cMesh* cubemesh = new cMesh();
	cMesh* terrainmesh = new cMesh();

	//if (!pModelLoader->loadModel("assets/models/Sky_Pirate_Combined_xyz.ply", piratemesh))
	//{
	//	std::cerr << "Failed to load Model" << std::endl;
	//	exit(EXIT_FAILURE);
	//}
	//
	if (!pModelLoader->loadModel("assets/models/terrain_xyzn.ply", terrainmesh))
	{
		std::cerr << "Failed to load Model" << std::endl;
		exit(EXIT_FAILURE);
	}
	//
	//if (!pModelLoader->loadModel("assets/models/bun_zipper_res4_xyz.ply", bunnymesh))
	//{
	//	std::cerr << "Failed to load Model" << std::endl;
	//	exit(EXIT_FAILURE);
	//}

	if (!pModelLoader->loadModel("assets/models/sphere_xyzn.ply", spheremesh))
	{
		std::cerr << "Failed to load Model" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!pModelLoader->loadModel("assets/models/cube_xyzn.ply", cubemesh))
	{
		std::cerr << "Failed to load Model" << std::endl;
		exit(EXIT_FAILURE);
	}

	// make shader
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
	GLuint view_loc = glGetUniformLocation(program, "matView");
	GLuint projection_loc = glGetUniformLocation(program, "matProjection");
	GLuint eyeLocation_loc = glGetUniformLocation(program, "eyeLocation");

#define getUniformLocationInArray(arr, i, d) glGetUniformLocation(program, arr "[" #i "]." d);

	// directional light (sun)
	cLight* light0 = new cLight();
	light0->position_loc =	getUniformLocationInArray("lights", 0, "position");
	light0->diffuse_loc =	getUniformLocationInArray("lights", 0, "diffuse");
	light0->specular_loc =	getUniformLocationInArray("lights", 0, "specular");
	light0->atten_loc =		getUniformLocationInArray("lights", 0, "atten");
	light0->direction_loc = getUniformLocationInArray("lights", 0, "direction");
	light0->param1_loc =	getUniformLocationInArray("lights", 0, "param1");
	light0->param2_loc =	getUniformLocationInArray("lights", 0, "param2");
	
	light0->position = glm::vec4(0, 0, 0, 1);
	light0->param2.x = 1.0f; // Set on
	
	light0->direction = glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f);

	light0->diffuse = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f);
	light0->param1.x = 2; // Directional light
	vecLights.push_back(light0);

	// SpotLight
	cLight* light1 = new cLight();
	light1->position_loc =	getUniformLocationInArray("lights", 1, "position");
	light1->diffuse_loc =	getUniformLocationInArray("lights", 1, "diffuse");
	light1->specular_loc =	getUniformLocationInArray("lights", 1, "specular");
	light1->atten_loc =		getUniformLocationInArray("lights", 1, "atten");
	light1->direction_loc = getUniformLocationInArray("lights", 1, "direction");
	light1->param1_loc =	getUniformLocationInArray("lights", 1, "param1");
	light1->param2_loc =	getUniformLocationInArray("lights", 1, "param2");

	light1->position = glm::vec4(0, 10, 0, 1);
	light1->param2.x = 1.0f; // Set on

	light1->param1.x = 0;
	light1->atten.y = 0.02f; // Linear attn
	light1->atten.z = 0.00f; // quadratic attn
	light1->atten.w = 1000000.0f; // Distance cutoff

	light1->diffuse = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	vecLights.push_back(light1);

	// Assign variables in shader
	for (unsigned i = 0; i < vecLights.size(); ++i)
	{
		glUniform4f(vecLights[i]->position_loc, vecLights[i]->position.x, vecLights[i]->position.y, vecLights[i]->position.z, 1.0f);
		glUniform4f(vecLights[i]->diffuse_loc, vecLights[i]->diffuse.r, vecLights[i]->diffuse.g, vecLights[i]->diffuse.b, 1.0f);
		glUniform4f(vecLights[i]->specular_loc, vecLights[i]->specular.r, vecLights[i]->specular.g, vecLights[i]->specular.b, 1.0f);
		glUniform4f(vecLights[i]->atten_loc, vecLights[i]->atten.x, vecLights[i]->atten.y, vecLights[i]->atten.z, vecLights[i]->atten.w);
		glUniform4f(vecLights[i]->direction_loc, vecLights[i]->direction.x, vecLights[i]->direction.y, vecLights[i]->direction.z, 1.0f);
		glUniform4f(vecLights[i]->param1_loc, vecLights[i]->param1.x, vecLights[i]->param1.y, vecLights[i]->param1.z, vecLights[i]->param1.w);
		glUniform4f(vecLights[i]->param2_loc, vecLights[i]->param2.x, vecLights[i]->param2.y, vecLights[i]->param2.z, vecLights[i]->param2.w);
	}

	cShaderManager::cShaderProgram* pShaderProgram = pShaderManager->pGetShaderProgramFromFriendlyName("shader01");
	
	//if (!pVAOManager->LoadModelIntoVAO("pirate", piratemesh, program))
	//{
	//	std::cerr << "Failed to load model to GPU" << std::endl;
	//	exit(EXIT_FAILURE);
	//}
	//
	if (!pVAOManager->LoadModelIntoVAO("terrain", terrainmesh, program))
	{
		std::cerr << "Failed to load model to GPU" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!pVAOManager->LoadModelIntoVAO("bunny", bunnymesh, program))
	{
		std::cerr << "Failed to load model to GPU" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!pVAOManager->LoadModelIntoVAO("sphere", spheremesh, program))
	{
		std::cerr << "Failed to load model to GPU" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (!pVAOManager->LoadModelIntoVAO("cube", cubemesh, program))
	{
		std::cerr << "Failed to load model to GPU" << std::endl;
		exit(EXIT_FAILURE);
	}

	cGameObject* debugSphere = new cGameObject("debugsphere");
	debugSphere->meshName = "sphere";
	debugSphere->position = glm::vec3(0.0f, 6.0f, 0.0f);
	debugSphere->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	debugSphere->scale = 1.0f;
	debugSphere->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	debugSphere->acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
	debugSphere->inverseMass = 0.0f;
	debugSphere->wireFrame = true;
	// Dont push to vecGameObjects

	cGameObject* cube = new cGameObject("cube");
	cube->meshName = "cube";
	cube->mesh = cubemesh;
	cube->position = glm::vec3(0.0f, 0.0f, 0.0f);
	cube->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	cube->scale = 1.0f;
	cube->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cube->inverseMass = 0.0f;
	vecGameObjects.push_back(cube);

	cGameObject* sphere = new cGameObject("sphere");
	sphere->meshName = "sphere";
	sphere->mesh = spheremesh;
	sphere->position = glm::vec3(0.0f, 6.0f, 0.0f);
	sphere->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	sphere->scale = 1.0f;
	sphere->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
	sphere->acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
	sphere->inverseMass = 1.0f;
	vecGameObjects.push_back(sphere);

	cGameObject* terrain = new cGameObject("terrain");
	terrain->meshName = "terrain";
	terrain->mesh = terrainmesh;
	terrain->position = glm::vec3(0.0f, -50.0f, 0.0f);
	terrain->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	terrain->scale = 1.0f;
	terrain->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	terrain->inverseMass = 0.0f;
	vecGameObjects.push_back(terrain);

	glm::vec3 lightPosition = glm::vec4(0.0f, 2.0f, 0.0f, 0.0f);
	//glUniform3f(lightpos_loc, lightPosition.x, lightPosition.y, lightPosition.z);
	float lightAtten = 1.0f;
	float ratio;
	int width, height;
	glm::mat4 m, v, p;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	// FOV, aspect ratio, near clip, far clip
	p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);

	glViewport(0, 0, width, height);
	
	glEnable(GL_DEPTH);			// Enable depth
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing
	//glUniform1f(atten_loc, lightAtten);	// default light value

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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Camera Movement
		glfwGetCursorPos(window, &cursorX, &cursorY);

		yaw += (cursorX - lastcursorX) * cameraSensitivity;
		pitch += (lastcursorY - cursorY) * cameraSensitivity;
		lastcursorX = cursorX;
		lastcursorY = cursorY;

		// Lock pitch
		if (pitch > 89.9f)
			pitch = 89.9f;
		else if (pitch < -89.9f)
			pitch = -89.9f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);


		if (!shift_pressed)
		{
			float cameraSpeed = CAMERA_SPEED;

			cameraEye += (mF - mB) * cameraSpeed * dt * cameraFront;
			cameraEye += (mR - mL) * cameraSpeed * dt * glm::normalize(glm::cross(cameraFront, cameraUp));
			cameraEye += (mU - mD) * cameraSpeed * dt * cameraUp;

			vecGameObjects[selectedObject]->scale += (rPress - fPress) * 1.0f * dt;
		}

		v = glm::mat4(1.0f);
		v = glm::lookAt(cameraEye, cameraEye + cameraFront, cameraUp);

		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
		//glUniform4f(eyeLocation_loc, cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);
		
		if (shift_pressed)
		{
			m = glm::mat4(1.0f);
			// Move light if shift pressed
			vecLights[selectedLight]->position.x += (mR - mL) * 3 * dt;
			vecLights[selectedLight]->position.y += (mU - mD) * 3 * dt;
			vecLights[selectedLight]->position.z += (mF - mB) * 3 * dt;
			vecLights[selectedLight]->atten.y *= ((fPress || rPress) ? ((fPress - rPress) * 0.01 + 1.00): 1.0); // Linear

			glUniform4f(vecLights[selectedLight]->position_loc, vecLights[selectedLight]->position.x, vecLights[selectedLight]->position.y, vecLights[selectedLight]->position.z, 1.0f);
			glUniform4f(vecLights[selectedLight]->diffuse_loc, vecLights[selectedLight]->diffuse.x, vecLights[selectedLight]->diffuse.y, vecLights[selectedLight]->diffuse.z, 1.0f);
			glUniform4f(vecLights[selectedLight]->specular_loc, 1.0f, 1.0f, 1.0f, 1.0f);	// White
			glUniform4f(vecLights[selectedLight]->atten_loc, 0.0f, vecLights[selectedLight]->atten.y, vecLights[selectedLight]->atten.z, vecLights[selectedLight]->atten.w);
			glUniform4f(vecLights[selectedLight]->direction_loc, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(vecLights[selectedLight]->param1_loc, 0.0f /*POINT light*/, 0.0f, 0.0f, 1.0f);
			glUniform4f(vecLights[selectedLight]->param2_loc, 1.0f /*Light is on*/, 0.0f, 0.0f, 1.0f);

			debugSphere->position = vecLights[selectedLight]->position;
			debugSphere->scale = 0.1f / vecLights[selectedLight]->atten.y;
			debugSphere->color = vecLights[selectedLight]->diffuse;
			// Draw light sphere if shift pressed
			drawObject(debugSphere, program, pVAOManager);
		}


		physicsUpdate(vecGameObjects, dt);


		std::ostringstream windowTitle;
		windowTitle << std::fixed << std::setprecision(2) << "Camera: Eye: {" << cameraEye.x << ", " << cameraEye.y << ", " << cameraEye.z << "} "
			<< "Front: {" << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << "} "
			<< "Selected: [" << selectedObject << "] \"" << vecGameObjects[selectedObject]->name << "\"";
		glfwSetWindowTitle(window, windowTitle.str().c_str());

		for (unsigned i = 0; i != vecGameObjects.size(); ++i)
		{
			if (!vecGameObjects[i]->visible)
				continue;

			drawObject(vecGameObjects[i], program, pVAOManager);
		}
		glfwSwapBuffers(window); // Draws to screen
		glfwPollEvents(); // Keyboard/Mouse input, etc.
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	

	// Delete everything

	for (auto g : vecGameObjects)
		delete g;
	for (auto l : vecLights)
		delete l;

	delete bunnymesh;
	//delete piratemesh;
	delete cubemesh;
	delete terrainmesh;
	delete pShaderManager;
	delete pModelLoader;
	delete pVAOManager;

	exit(EXIT_SUCCESS);
}

// Draw an object
void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager)
{
	glm::mat4 m = glm::mat4(1.0f);

	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), go->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), go->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), go->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(go->scale, go->scale, go->scale));
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), go->position);

	m *= rotationX * rotationY * rotationZ * translation * scale;

	glUniformMatrix4fv(glGetUniformLocation(shader, "matModel"), 1, GL_FALSE, glm::value_ptr(m));
	glUniform4f(glGetUniformLocation(shader, "diffuseColour"), go->color.r, go->color.g, go->color.b, go->color.a);

	if (go->wireFrame)
	{
		glUniform1f(glGetUniformLocation(shader, "bDoNotLight"), (float)GL_TRUE);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glUniform1f(glGetUniformLocation(shader, "bDoNotLight"), (float)GL_FALSE);
		glEnable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	sModelDrawInfo drawInfo;
	if (pVAOManager->FindDrawInfoByModelName(go->meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}
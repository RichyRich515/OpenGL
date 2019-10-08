// Main.cpp
// Entrypoint for program
// 2019-09-04

#include "GLCommon.h"

#include <stdlib.h>
#include <stdio.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <json/json.h>

#include "cMesh.hpp"
#include "cModelLoader.hpp"
#include "cVAOManager.hpp"
#include "cShaderManager.hpp"
#include "cGameObject.hpp"
#include "cLight.hpp"
#include "Physics.hpp"

#include "DebugRenderer/cDebugRenderer.h"

cDebugRenderer* debugRenderer;


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraEye = glm::vec3(0.0f, 28.0f, 10.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
float pitch = 0.0f; // X axis rotation ( up, down)
float yaw = -90.0f; // Y axis rotation (left, right)
glm::vec3 cameraDirection = glm::normalize(cameraEye - cameraFront);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

float CAMERA_SPEED = 3.0f;

float cameraSensitivity = 0.1f;

// TODO: THis outta global space
GLuint program = 0;

std::map<std::string, cMesh*> mapMeshes;

// List of all game objects in scene
std::vector<cGameObject*> vecGameObjects;
int selectedObject = 0;


// Max lights from the shader
constexpr unsigned MAX_LIGHTS = 10;

// List of all lights in scene
std::vector<cLight*> vecLights;
int selectedLight = 2;


// write current scene to a file
void writeSceneToFile(std::string filename)
{
	std::ofstream ofs(filename);

	Json::Value root;
	root["world"] = Json::objectValue;
	root["camera"] = Json::objectValue;
	root["gameObjects"] = Json::arrayValue;
	root["lights"] = Json::arrayValue;

	// TODO: ambience and gravity properly
	glm::vec4 ambience = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glm::vec3 gravity = glm::vec3(0.0f, -1.0f, 0.0f);
	for (unsigned i = 0; i < 3; ++i) // vec 3s
	{
		root["camera"]["cameraEye"][i] = cameraEye[i];
		root["camera"]["cameraUp"][i] = cameraUp[i];
		root["camera"]["cameraFront"][i] = cameraFront[i];
		root["world"]["gravity"][i] = gravity[i];
	}
	for (unsigned i = 0; i < 4; ++i) // vec 4s
	{
		root["world"]["ambience"][i] = ambience[i];
	}

	root["camera"]["pitch"] = pitch;
	root["camera"]["yaw"] = yaw;

	for (auto g : vecGameObjects)
		root["gameObjects"].append(g->serializeJSONObject());

	for (auto l : vecLights)
		root["lights"].append(l->serializeJSONObject());

	ofs << root;
}

void openSceneFromFile(std::string filename)
{
	glUseProgram(program);
	selectedObject = 0;
	selectedLight = 0;
	std::ifstream scenejson(filename);
	Json::Value root;
	scenejson >> root;
	scenejson.close();

	Json::Value world = root["world"];
	Json::Value camera = root["camera"];
	Json::Value lights = root["lights"];
	Json::Value gameObjects = root["gameObjects"];

	for (unsigned i = 0; i < 3; ++i)
	{
		cameraEye[i] = camera["cameraEye"][i].asFloat();
		cameraUp[i] = camera["cameraUp"][i].asFloat();
		cameraFront[i] = camera["cameraFront"][i].asFloat();
	}
	cameraDirection = glm::normalize(cameraEye - cameraFront);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	pitch = camera["pitch"].asFloat();
	yaw = camera["yaw"].asFloat();

	// TODO: move this somewhere else???
	// Ambient Light (Imagine all the sunlight bounced around and evenly lit everything)
	Json::Value ambience = world["ambience"];
	glUniform4f(glGetUniformLocation(program, "ambientColour"), ambience[0].asFloat(), ambience[1].asFloat(), ambience[2].asFloat(), ambience[3].asFloat());


	for (auto g : vecGameObjects)
		delete g;
	vecGameObjects.clear();

	for (auto l : vecLights)
		delete l;
	vecLights.clear();

	for (unsigned i = 0; i < lights.size() && i < MAX_LIGHTS; ++i)
	{
		cLight* l = new cLight(i, lights[i], program);
		vecLights.push_back(l);
	}

	for (unsigned i = 0; i < gameObjects.size(); ++i)
	{
		cGameObject* go = new cGameObject(gameObjects[i], mapMeshes);
		vecGameObjects.push_back(go);
	}
}


// TODO: keyboard manager
bool ctrl_pressed = false, shift_pressed = false,
mF = false, mB = false, mL = false, mR = false,
mU = false, mD = false, rPress = false, fPress = false;

bool kJ = false, kL = false, kI = false, kK = false, kU = false, kO = false;

bool debug_mode = false;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	else if (key == GLFW_KEY_LEFT_SHIFT)
	{
		if (action == GLFW_PRESS)
			shift_pressed = true;
		else if (action == GLFW_RELEASE)
			shift_pressed = false;
	}
	else if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
	{
		if (action == GLFW_PRESS)
			ctrl_pressed = true;
		else if (action == GLFW_RELEASE)
			ctrl_pressed = false;
	}
	else if (key == GLFW_KEY_GRAVE_ACCENT)
	{
		if (action == GLFW_PRESS)
			debug_mode = !debug_mode;
	}

	else if (key == GLFW_KEY_W)
		if (action == GLFW_PRESS)
			mF = true;
		else if (action == GLFW_RELEASE)
			mF = false;

	else if (key == GLFW_KEY_S)
		if (action == GLFW_PRESS)
			mB = true;
		else if (action == GLFW_RELEASE)
			mB = false;

	else if (key == GLFW_KEY_A)
		if (action == GLFW_PRESS)
			mL = true;
		else if (action == GLFW_RELEASE)
			mL = false;

	else if (key == GLFW_KEY_D)
		if (action == GLFW_PRESS)
			mR = true;
		else if (action == GLFW_RELEASE)
			mR = false;

	else if (key == GLFW_KEY_SPACE)
		if (action == GLFW_PRESS)
			mU = true;
		else if (action == GLFW_RELEASE)
			mU = false;

	else if (key == GLFW_KEY_C)
		if (action == GLFW_PRESS)
			mD = true;
		else if (action == GLFW_RELEASE)
			mD = false;

	else if (key == GLFW_KEY_R)
		if (action == GLFW_PRESS)
			rPress = true;
		else if (action == GLFW_RELEASE)
			rPress = false;

	else if (key == GLFW_KEY_F)
		if (action == GLFW_PRESS)
			fPress = true;
		else if (action == GLFW_RELEASE)
			fPress = false;

	else if (key == GLFW_KEY_J)
		if (action == GLFW_PRESS)
			kJ = true;
		else if (action == GLFW_RELEASE)
			kJ = false;


	else if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
	{
		if (shift_pressed)
		{
			++selectedLight;
			if (selectedLight >= vecLights.size())
				selectedLight = 0;
		}
		else
		{
			++selectedObject;
			if (selectedObject >= vecGameObjects.size())
				selectedObject = 0;
		}
	}
	else if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
	{
		if (shift_pressed)
		{
			--selectedLight;
			if (selectedLight < 0)
				selectedLight = (int)vecLights.size() - 1;
		}
		else
		{
			--selectedObject;
			if (selectedObject < 0)
				selectedObject = (int)vecGameObjects.size() - 1;
		}
	}

	else if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		if (shift_pressed)
		{
			vecLights[selectedLight]->param2.x = !vecLights[selectedLight]->param2.x;
		}
		else
		{
			vecGameObjects[selectedObject]->visible = !vecGameObjects[selectedObject]->visible;
		}
	}

	else if (key == GLFW_KEY_K)
	{
		vecLights[selectedLight]->param1.z -= 0.1f;
	}

	else if (key == GLFW_KEY_L)
	{
		vecLights[selectedLight]->param1.z += 0.1f;
	}

	else if (ctrl_pressed && key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		// Open scene
		openSceneFromFile("scene2.json");
	}
	else if (ctrl_pressed && key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		// Print scene (to file)
		writeSceneToFile("scene2.json");
	}
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

	window = glfwCreateWindow(1920, 1080, "", NULL, NULL);
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

	debugRenderer = new cDebugRenderer();
	debugRenderer->initialize();

	// TODO: Constant strings
	//		 Make a class for this??
	// Read scene file
	std::ifstream modelsjson("models.json");
	Json::Value models;
	modelsjson >> models;
	modelsjson.close();

	for (unsigned i = 0; i < models.size(); ++i)
	{
		std::string name = models[i]["name"].asString();
		std::string location = models[i]["location"].asString();

		cMesh* m = new cMesh();
		if (!pModelLoader->loadModel(location, m))
		{
			std::cerr << "Failed to load Model \"" << name << "\" from " << location << std::endl;
			exit(EXIT_FAILURE);
		}
		mapMeshes[name] = m;
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

	program = pShaderManager->getIDFromFriendlyName("shader01");
	glUseProgram(program);
	GLuint view_loc = glGetUniformLocation(program, "matView");
	GLuint projection_loc = glGetUniformLocation(program, "matProjection");
	GLuint eyeLocation_loc = glGetUniformLocation(program, "eyeLocation");

	for (auto m : mapMeshes)
	{
		if (!pVAOManager->LoadModelIntoVAO(m.first, m.second, program))
		{
			std::cerr << "Failed to load model " << m.first << " to GPU" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	openSceneFromFile("scene1.json");

	cGameObject* debugSphere = new cGameObject("debugsphere");
	debugSphere->meshName = "sphere";
	debugSphere->inverseMass = 0.0f;
	debugSphere->wireFrame = true;

	// TODO: delete this temp sphere creation stuff
	unsigned sx = 5;
	unsigned sz = 2;
	for (unsigned x = 0; x < sx; x++)
	{
		for (unsigned z = 0; z < sz; z++)
		{
			std::ostringstream name;
			name << "sphere" << x << z;
			cGameObject* sphere = new cGameObject(name.str());
			sphere->meshName = "sphere";
			sphere->mesh = mapMeshes["sphere"];
			sphere->position = glm::vec3(x - (sx / 2.0f), 30.0f, z - (sz / 2.0f));
			sphere->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
			sphere->scale = 1.0f;
			sphere->color = glm::vec4(1.0f - (x / (float)sx), 0.2f, 1.0f - (z / (float)sz), 1.0f);
			sphere->specular = glm::vec4(1.0f, 1.0f, 1.0f, 10.0f);
			sphere->acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
			sphere->inverseMass = 1.0f;
			sphere->bounciness = 0.8f;

			sphere->collisionShapeType = SPHERE;
			sphere->collisionObjectInfo.radius = 0.5f;

			vecGameObjects.push_back(sphere);
		}
	}

	float ratio;
	int width, height;
	glm::mat4 v, p;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	// FOV, aspect ratio, near clip, far clip
	p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH);			// Enable depth
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	// timing
	float totalTime;
	float lastTime = 0;
	float dt;

	double cursorX = 0, cursorY = 0, lastcursorX = 0, lastcursorY = 0;

	glfwGetCursorPos(window, &cursorX, &cursorY);
	lastcursorX = cursorX;
	lastcursorY = cursorY;


	while (!glfwWindowShouldClose(window))
	{
		totalTime = glfwGetTime();
		dt = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();

		glUseProgram(program);
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


		if (ctrl_pressed)
		{
			glm::vec3 velocity = dt * CAMERA_SPEED * glm::vec3(mR - mL, mU - mD, mF - mB);
			vecGameObjects[selectedObject]->translate(velocity);
			vecGameObjects[selectedObject]->scale += (rPress - fPress) * 1.0f * dt;
		}
		else if (!shift_pressed)
		{
			float cameraSpeed = CAMERA_SPEED;

			cameraEye += (mF - mB) * cameraSpeed * dt * cameraFront;
			cameraEye += (mR - mL) * cameraSpeed * dt * glm::normalize(glm::cross(cameraFront, cameraUp));
			cameraEye += (mU - mD) * cameraSpeed * dt * cameraUp;
		}

		v = glm::mat4(1.0f);
		v = glm::lookAt(cameraEye, cameraEye + cameraFront, cameraUp);

		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
		glUniform4f(eyeLocation_loc, cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);

		if (shift_pressed)
		{
			glm::mat4 m = glm::mat4(1.0f);
			float speed = 3.0f;
			// Move light if shift pressed
			vecLights[selectedLight]->position.x += (mR - mL) * speed * dt;
			vecLights[selectedLight]->position.y += (mU - mD) * speed * dt;
			vecLights[selectedLight]->position.z += (mF - mB) * speed * dt;
			vecLights[selectedLight]->atten.y *= ((fPress || rPress) ? ((fPress - rPress) * 0.01f + 1.0f) : 1.0f); // Linear


			vecLights[selectedLight]->updateShaderUniforms();

			debugSphere->position = vecLights[selectedLight]->position;
			debugSphere->scale = 0.1f / vecLights[selectedLight]->atten.y;
			debugSphere->color = vecLights[selectedLight]->diffuse;
			// Draw light sphere if shift pressed
			drawObject(debugSphere, program, pVAOManager);
		}

		// collisions and stuff
		physicsUpdate(vecGameObjects, dt, debugRenderer, debug_mode);

		std::ostringstream windowTitle;
		windowTitle << std::fixed << std::setprecision(2) << "Camera: Eye: {" << cameraEye.x << ", " << cameraEye.y << ", " << cameraEye.z << "} "
			<< "Front: {" << cameraFront.x << ", " << cameraFront.y << ", " << cameraFront.z << "} "
			<< "Selected: [" << selectedObject << "] \"" << vecGameObjects[selectedObject]->name << "\" "
			<< "Light: [" << selectedLight << "]";
		glfwSetWindowTitle(window, windowTitle.str().c_str());

		//debugRenderer->addLine(glm::vec3(-10, 25, 0), glm::vec3(10, 32, 0), glm::vec3(0, 1, 0));
		for (unsigned i = 0; i != vecGameObjects.size(); ++i)
		{
			if (!vecGameObjects[i]->visible)
				continue;

			drawObject(vecGameObjects[i], program, pVAOManager);
		}

		if (debug_mode)
		{
			debugRenderer->RenderDebugObjects(v, p, dt);
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

	for (auto m : mapMeshes)
		delete m.second;

	delete pShaderManager;
	delete pModelLoader;
	delete pVAOManager;
	delete debugRenderer;

	exit(EXIT_SUCCESS);
}


// Draw an object
void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager)
{
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), go->position);
	glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), go->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), go->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), go->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(go->scale, go->scale, go->scale));

	go->matWorld = glm::mat4(1.0f) * rotationX * rotationY * rotationZ * translation * scale;
	go->inverseTransposeMatWorld = glm::inverse(glm::transpose(go->matWorld));
	if (go->collisionShapeType == MESH)
		go->calculateCollisionMeshTransformed();

	glUniformMatrix4fv(glGetUniformLocation(shader, "matModel"), 1, GL_FALSE, glm::value_ptr(go->matWorld));
	glUniformMatrix4fv(glGetUniformLocation(shader, "matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(go->inverseTransposeMatWorld));
	glUniform4f(glGetUniformLocation(shader, "diffuseColour"), go->color.r, go->color.g, go->color.b, go->color.a);
	glUniform4f(glGetUniformLocation(shader, "specularColour"), go->specular.r, go->specular.g, go->specular.b, go->specular.a);
	glUniform1f(glGetUniformLocation(shader, "bDoNotLight"), (float)go->wireFrame);

	if (go->wireFrame)
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
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
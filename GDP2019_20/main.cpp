// Main.cpp
// Entrypoint for program
// Richard Woods
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
#include "Texture/cBasicTextureManager.h"
#include "cVAOManager.hpp"
#include "cShaderManager.hpp"
#include "cGameObject.hpp"
#include "cPelican.hpp"
#include "cLight.hpp"
#include "Physics.hpp"
#include "cKeyboardManager.hpp"

#include "iGameObjectFactory.hpp"
#include "cFactoryManager.hpp"

#include "DebugRenderer/cDebugRenderer.h"

#include "cWorld.hpp"

#include "cParticleEmitter.hpp"


constexpr float MAX_DELTA_TIME = 0.017f;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager, float dt, float tt);

glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraEye = glm::vec3(0.0f, 28.0f, 10.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
float pitch = 0.0f; // X axis rotation ( up, down)
float yaw = -90.0f; // Y axis rotation (left, right)
glm::vec3 cameraDirection = glm::normalize(cameraEye - cameraFront);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));

float CAMERA_SPEED = 25.0f;

float cameraSensitivity = 0.1f;

// TODO: Get this outta global space
GLuint program = 0;

std::map<std::string, cMesh*> mapMeshes;


// factory for any game object
iGameObjectFactory* pGameObjectFactory;
cBasicTextureManager* pTextureManager;

int selectedObject = 0;
int selectedLight = 0;

// Max lights from the shader
constexpr unsigned MAX_LIGHTS = 10;

cWorld* world = nullptr;

glm::vec3 gravity;
glm::vec4 ambience;

// write current scene to a file
void writeSceneToFile(std::string filename)
{
	std::ofstream ofs(filename);

	Json::Value root;
	root["world"] = Json::objectValue;
	root["camera"] = Json::objectValue;
	root["gameObjects"] = Json::arrayValue;
	root["lights"] = Json::arrayValue;

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

	root["world"]["factoryType"] = pGameObjectFactory->getTypeString();

	root["camera"]["pitch"] = pitch;
	root["camera"]["yaw"] = yaw;

	for (auto g : world->vecGameObjects)
		root["gameObjects"].append(g->serializeJSONObject());

	for (auto l : world->vecLights)
		root["lights"].append(l->serializeJSONObject());

	ofs << root;
	std::cout << "Saved scene to " << filename << std::endl;
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

	Json::Value world_node = root["world"];
	Json::Value camera = root["camera"];
	Json::Value lights = root["lights"];
	Json::Value gameObjects = root["gameObjects"];

	for (unsigned i = 0; i < 3; ++i)
	{
		cameraEye[i] = camera["cameraEye"][i].asFloat();
		cameraUp[i] = camera["cameraUp"][i].asFloat();
		cameraFront[i] = camera["cameraFront"][i].asFloat();
		gravity[i] = world_node["gravity"][i].asFloat();
	}
	cameraDirection = glm::normalize(cameraEye - cameraFront);
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	pitch = camera["pitch"].asFloat();
	yaw = camera["yaw"].asFloat();

	Json::Value ambienceobj = world_node["ambience"];
	for (unsigned i = 0; i < 4; ++i) // vec 4s
	{
		ambience[i] = ambienceobj[i].asFloat();
	}
	glUniform4f(glGetUniformLocation(program, "ambientColour"), ambience[0], ambience[1], ambience[2], ambience[3]);

	if (pGameObjectFactory)
		delete pGameObjectFactory;
	pGameObjectFactory = cFactoryManager::getObjectFactory(world_node["factoryType"].asString());
	if (!pGameObjectFactory)
	{
		std::cout << "Invalid factory [" << world_node["factoryType"].asString() << "]\nexiting" << std::endl;
		exit(EXIT_FAILURE);
		// TODO: cleanup??
	}

	for (auto g : world->vecGameObjects)
		delete g;
	world->vecGameObjects.clear();

	for (auto l : world->vecLights)
	{
		// reset lights in shader so they don't stick around
		l->param2.x = 0;
		l->updateShaderUniforms();
		delete l;
	}

	world->vecLights.clear();

	for (unsigned i = 0; i < lights.size() && i < MAX_LIGHTS; ++i)
	{
		cLight* l = new cLight(i, lights[i], program);
		world->vecLights.push_back(l);
	}


	for (unsigned i = 0; i < gameObjects.size(); ++i)
	{
		cGameObject* go = pGameObjectFactory->createFromJSON(gameObjects[i], mapMeshes);
		world->vecGameObjects.push_back(go);
	}


	for (auto go : world->vecGameObjects)
		go->init();

	std::cout << "Opened scene " << filename << std::endl;
}

bool ctrl_pressed = false, shift_pressed = false;

bool debug_mode = false;

cKeyboardManager* pKeyboardManager = NULL;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	pKeyboardManager->handleKeyboardInput(key, scancode, action, mods);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_LEFT_SHIFT)
	{
		if (action == GLFW_PRESS)
			shift_pressed = true;
		else if (action == GLFW_RELEASE)
			shift_pressed = false;
	}
	if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL)
	{
		if (action == GLFW_PRESS)
			ctrl_pressed = true;
		else if (action == GLFW_RELEASE)
			ctrl_pressed = false;
	}
}

int main()
{
	// 19x3x19
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
	pKeyboardManager = new cKeyboardManager();
	world = cWorld::getWorld();

	cDebugRenderer* pDebugRenderer = new cDebugRenderer();
	pDebugRenderer->initialize();

	// TODO: Constant strings
	//		 Make a class for this??
	// load all models
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
	//glUseProgram(program);
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

	pTextureManager = new cBasicTextureManager();
	pTextureManager->SetBasePath("assets/textures");

	std::ifstream texturesjson("textures.json");
	Json::Value textures;
	texturesjson >> textures;
	texturesjson.close();

	// Textures
	for (unsigned i = 0; i < textures["textures"].size(); ++i)
	{
		if (!pTextureManager->Create2DTextureFromBMPFile(textures["textures"][i].asString(), true)) // NEED TO GENERATE MIP MAPS
		{
			std::cerr << "Failed to load texture " << textures["textures"][i].asString() << " to GPU" << std::endl;
		}
	}

	// Skyboxes
	for (unsigned i = 0; i < textures["skyboxes"].size(); ++i)
	{
		std::string err;
		if (!pTextureManager->CreateCubeTextureFromBMPFiles(
			textures["skyboxes"][i]["name"].asString(),
			textures["skyboxes"][i]["file_left"].asString(),
			textures["skyboxes"][i]["file_right"].asString(),
			textures["skyboxes"][i]["file_down"].asString(),
			textures["skyboxes"][i]["file_up"].asString(),
			textures["skyboxes"][i]["file_front"].asString(),
			textures["skyboxes"][i]["file_back"].asString(),
			true, err)) // NEED TO GENERATE MIP MAPS
		{
			std::cerr << "Failed to load cubemap " << textures["skyboxes"][i]["name"].asString() << " to GPU: " << err << std::endl;
		}
	}

	cGameObject* debugSphere = new cGameObject("debugsphere");
	debugSphere->meshName = "sphere";
	debugSphere->inverseMass = 0.0f;
	debugSphere->wireFrame = true;
	debugSphere->lighting = true;


	openSceneFromFile("scene1.json");



	cParticleEmitter emitter1;
	emitter1.init(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f), glm::vec3(0.0f),
		glm::vec3(-0.01f), glm::vec3(0.01f),
		0.5f, 2.0f,
		glm::vec4(0.7f), glm::vec4(0.3f),
		0.025f, 0.0f,
		5, 20,
		1000);

	cParticleEmitter emitter2;
	emitter2.init(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f), glm::vec3(0.0f),
		glm::vec3(-0.01f), glm::vec3(0.01f),
		0.5f, 2.0f,
		glm::vec4(0.7f), glm::vec4(0.3f),
		0.025f, 0.0f,
		5, 20,
		1000);


	float ratio;
	int width, height;
	glm::mat4 v, p;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	float fov = 60.0f;
	float minfov = 60.0f;

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH);			// Enable depth
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.7f, 0.7f, 1.0f, 1.0f);

	// timing
	float totalTime;
	float lastTime = 0;
	float dt;

	double cursorX, cursorY;
	float lastcursorX = 0, lastcursorY = 0;

	while (!glfwWindowShouldClose(window))
	{
		totalTime = (float)glfwGetTime();
		dt = totalTime - lastTime;
		lastTime = totalTime;

		if (dt >= MAX_DELTA_TIME)
			dt = MAX_DELTA_TIME;

		// Camera Movement
		glfwGetCursorPos(window, &cursorX, &cursorY);

		yaw += ((float)cursorX - lastcursorX) * cameraSensitivity;
		pitch += (lastcursorY - (float)cursorY) * cameraSensitivity;
		lastcursorX = (float)cursorX;
		lastcursorY = (float)cursorY;

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

		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (pKeyboardManager->keyPressed(GLFW_KEY_GRAVE_ACCENT))
		{
			debug_mode = !debug_mode;
		}

		int xMove = pKeyboardManager->keyDown(GLFW_KEY_A) - pKeyboardManager->keyDown(GLFW_KEY_D);
		int yMove = pKeyboardManager->keyDown(GLFW_KEY_SPACE) - pKeyboardManager->keyDown(GLFW_KEY_C);
		int zMove = pKeyboardManager->keyDown(GLFW_KEY_W) - pKeyboardManager->keyDown(GLFW_KEY_S);

		cameraEye += zMove * CAMERA_SPEED * dt * cameraFront;
		cameraEye += -xMove * CAMERA_SPEED * dt * glm::normalize(glm::cross(cameraFront, cameraUp));
		cameraEye += yMove * CAMERA_SPEED * dt * cameraUp;

		if (world->vecGameObjects.size() && world->vecLights.size())
		{
			std::ostringstream windowTitle;
			windowTitle << std::fixed << std::setprecision(2)
				<< "{" << cameraEye.x << ", " << cameraEye.y << ", " << cameraEye.z << "}";
			glfwSetWindowTitle(window, windowTitle.str().c_str());
		}

		world->vecGameObjects[4]->textures[0].xOffset = -totalTime / 20.0f;
		world->vecGameObjects[4]->textures[0].yOffset = totalTime / 10.0f;

		world->vecGameObjects[4]->heightmap.xOffset = -totalTime / 30.0f;
		world->vecGameObjects[4]->heightmap.yOffset = totalTime / 20.0f;
		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			world->vecGameObjects[i]->update(dt);
			world->vecGameObjects[i]->physicsUpdate(dt);
			world->vecGameObjects[i]->updateMatricis();
		}

		// FOV, aspect ratio, near clip, far clip
		p = glm::perspective(glm::radians(fov), ratio, 0.1f, 1000.0f);
		v = glm::lookAt(cameraEye, cameraEye + cameraFront, cameraUp);

		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
		glUniform4f(eyeLocation_loc, cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);


		// draw Skybox
		{
			// Tie texture
			GLuint texture_ul = pTextureManager->getTextureIDFromName("snowy_mountains");
			if (texture_ul)
			{
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ul);
				glUniform1i(glGetUniformLocation(program, "skyboxSamp00"), 10);
			}
			debugSphere->scale = 1.0f;
			debugSphere->position = cameraEye;
			debugSphere->wireFrame = false;
			//debugSphere->lighting = true;
			debugSphere->color = glm::vec4(1.0f);
			//debugSphere->visible = true;
			//debugSphere->calculateCollisionMeshTransformed();
			debugSphere->updateMatricis();
			glUniformMatrix4fv(glGetUniformLocation(program, "matModel"), 1, GL_FALSE, glm::value_ptr(debugSphere->matWorld));
			glUniformMatrix4fv(glGetUniformLocation(program, "matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(debugSphere->inverseTransposeMatWorld));
			glUniform4f(glGetUniformLocation(program, "diffuseColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(glGetUniformLocation(program, "specularColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(glGetUniformLocation(program, "params1"), dt, totalTime, 1.0f, 0.0f);
			glUniform4f(glGetUniformLocation(program, "params2"), 1.0f, 0.0f, 0.0f, 0.0f);
			glUniform4f(glGetUniformLocation(program, "heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH);
			glDisable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			sModelDrawInfo drawInfo;
			if (pVAOManager->FindDrawInfoByModelName(debugSphere->meshName, drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH);
			glEnable(GL_DEPTH_TEST);
		}

		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			if (!world->vecGameObjects[i]->visible)
				continue;
			drawObject(world->vecGameObjects[i], program, pVAOManager, dt, totalTime);
		}

		if (debug_mode)
		{
			pDebugRenderer->RenderDebugObjects(v, p, dt);
		}

		glfwSwapBuffers(window); // Draws to screen
		glfwPollEvents(); // Keyboard/Mouse input, etc.

		world->doDeferredActions();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	// Delete everything
	for (auto g : world->vecGameObjects)
		delete g;

	for (auto l : world->vecLights)
		delete l;

	for (auto m : mapMeshes)
		delete m.second;

	delete pShaderManager;
	delete pModelLoader;
	delete pVAOManager;
	delete pKeyboardManager;
	delete pDebugRenderer;

	delete debugSphere;
	exit(EXIT_SUCCESS);
}


// Draw an object
void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager, float dt, float tt)
{
	// Tie texture
	GLuint texture_ul = pTextureManager->getTextureIDFromName(go->textures[0].fileName);
	if (texture_ul)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texture_ul);
		glUniform1i(glGetUniformLocation(shader, "textSamp00"), 0);
		glUniform4f(glGetUniformLocation(shader, "textparams00"),
			go->textures[0].xOffset,
			go->textures[0].yOffset,
			go->textures[0].blend,
			go->textures[0].tiling);
	}
	else
	{
		glUniform4f(glGetUniformLocation(shader, "textparams00"), 0.0f, 0.0f, 0.0f, 0.0f);
	}
	texture_ul = pTextureManager->getTextureIDFromName(go->heightmap.fileName);
	if (texture_ul)
	{
		glActiveTexture(GL_TEXTURE0 + 40);
		glBindTexture(GL_TEXTURE_2D, texture_ul);
		glUniform1i(glGetUniformLocation(shader, "heightSamp"), 40);
		glUniform4f(glGetUniformLocation(shader, "heightparams"),
			go->heightmap.xOffset,
			go->heightmap.yOffset,
			go->heightmap.blend,
			go->heightmap.tiling);
	}
	else
	{
		glUniform4f(glGetUniformLocation(shader, "heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);
	}

	glUniformMatrix4fv(glGetUniformLocation(shader, "matModel"), 1, GL_FALSE, glm::value_ptr(go->matWorld));
	glUniformMatrix4fv(glGetUniformLocation(shader, "matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(go->inverseTransposeMatWorld));
	glUniform4f(glGetUniformLocation(shader, "diffuseColour"), go->color.r, go->color.g, go->color.b, go->color.a);
	glUniform4f(glGetUniformLocation(shader, "specularColour"), go->specular.r, go->specular.g, go->specular.b, go->specular.a);
	glUniform4f(glGetUniformLocation(shader, "params1"), dt, tt, (float)go->lighting, (float)texture_ul);
	glUniform4f(glGetUniformLocation(shader, "params2"), 0.0f, 0.0f, 0.0f, 0.0f);

	if (go->wireFrame)
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		// glCullFace
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
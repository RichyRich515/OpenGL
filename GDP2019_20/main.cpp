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
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <json/json.h>
#include "JsonHelper.hpp"

#include "cCamera.hpp"
#include "cMesh.hpp"
#include "cModelLoader.hpp"
#include "cVAOManager.hpp"
#include "cShaderManager.hpp"

#include "iGameObject.hpp"
#include "cGameObject.hpp"
#include "cPhysicsGameObject.hpp"
#include "cAnimatedGameObject.hpp"
#include "cLight.hpp"
#include "cKeyboardManager.hpp"

#include "iGameObjectFactory.hpp"
#include "cFactoryManager.hpp"

#include "DebugRenderer/cDebugRenderer.h"

#include "cWorld.hpp"

#include "cParticleEmitter.hpp"

#include "cFBO.h"

#include "cPhysicsManager.hpp"

#include "Texture/cBasicTextureManager.h"

#define MY_PHYSICS

#ifdef MY_PHYSICS
constexpr char physics_library_name[21] = "MyPhysicsWrapper.dll";
#else
constexpr char physics_library_name[25] = "BulletPhysicsWrapper.dll";
#endif

cPhysicsManager* pPhysicsManager;

namespace std
{
	inline std::string to_string(glm::vec3 _Val)
	{
		return std::string("{" + std::to_string(_Val.x) + ", " + std::to_string(_Val.y) + ", " + std::to_string(_Val.z) + "}");
	}
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

//void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager, float dt, float tt);

constexpr float MAX_DELTA_TIME = 0.017f;

cCamera* camera;
cCamera* camera_inTV;
cCamera* camera_inSpace;


// TODO: Get this outta global space
cShaderManager::cShaderProgram* pShader;
GLuint program = 0;

std::map<std::string, cMesh*> mapMeshes;

cFBO* fbo = nullptr;
cFBO* fbo2 = nullptr;

// factory for any game object
iGameObjectFactory* pGameObjectFactory;
cBasicTextureManager* pTextureManager;

int selectedObject = 0;
int selectedLight = 0;

// Max lights from the shader
constexpr unsigned MAX_LIGHTS = 20;

cWorld* world = cWorld::getWorld();

glm::vec3 gravity;
glm::vec4 ambience = glm::vec4(0.25f);

void writeSceneToFile(std::string filename)
{
	std::ofstream ofs(filename);

	Json::Value root;
	root["world"] = Json::objectValue;
	root["camera"] = Json::objectValue;
	root["gameObjects"] = Json::arrayValue;
	root["lights"] = Json::arrayValue;

	// TODO: rewrite?
	for (unsigned i = 0; i < 3; ++i) // vec 3s
	{
		root["camera"]["cameraEye"][i] = camera->position[i];
		root["world"]["gravity"][i] = gravity[i];
	}
	for (unsigned i = 0; i < 4; ++i) // vec 4s
	{
		root["world"]["ambience"][i] = ambience[i];
	}

	root["world"]["factoryType"] = pGameObjectFactory->getTypeString();

	root["camera"]["pitch"] = camera->pitch;
	root["camera"]["yaw"] = camera->yaw;
	root["camera"]["cameraSpeed"] = camera->speed;

	/*for (auto g : world->vecGameObjects)
		root["gameObjects"].append(g->serializeJSONObject());*/

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
	Json::Value camera_node = root["camera"];
	Json::Value lights = root["lights"];
	Json::Value gameObjects = root["gameObjects"];

	for (unsigned i = 0; i < 3; ++i)
	{
		camera->position[i] = camera_node["cameraEye"][i].asFloat();
		//gravity[i] = world_node["gravity"][i].asFloat();
	}
	camera->pitch = camera_node["pitch"].asFloat();
	camera->yaw = camera_node["yaw"].asFloat();
	camera->speed = camera_node["cameraSpeed"].asFloat();

	Json::Value ambienceobj = world_node["ambience"];
	for (unsigned i = 0; i < 4; ++i) // vec 4s
	{
		ambience[i] = ambienceobj[i].asFloat();
	}

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

	int idCounter = 0;
	for (unsigned i = 0; i < gameObjects.size(); ++i, ++idCounter)
	{
		iGameObject* go = pGameObjectFactory->createFromJSON(gameObjects[i], mapMeshes);
		go->id = idCounter;
		world->vecGameObjects.push_back(go);
	}

	for (auto go : world->vecGameObjects)
		go->init();

	std::cout << "Opened scene " << filename << std::endl;
}

bool ctrl_pressed = false, shift_pressed = false;

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
	std::cout << "start" << std::endl;
	srand(time(NULL));
	GLFWwindow* window;

	// GLFW setup
	{
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
	}

	cModelLoader* pModelLoader = new cModelLoader();
	cVAOManager* pVAOManager = new cVAOManager();
	cVAOManager::setCurrentVAOManager(pVAOManager);
	cShaderManager* pShaderManager = new cShaderManager();
	GLuint view_loc = 0;
	GLuint projection_loc = 0;
	GLuint eyeLocation_loc = 0;

	// load models to memory
	{
		std::ifstream modelsjson("assets/models.json");
		Json::Value models;
		modelsjson >> models;
		modelsjson.close();
		std::string error_string;
		for (unsigned i = 0; i < models.size(); ++i)
		{
			std::string name = models[i]["name"].asString();
			std::string location = models[i]["location"].asString();
			std::cout << "Loading model: " << name << std::endl;
			cMesh* m = new cMesh();
			if (!pModelLoader->loadModel_assimp(location, m, error_string))
			{
				std::cerr << "Failed to load Model \"" << name << "\" from " << location << " Error: " << error_string << std::endl;
				exit(EXIT_FAILURE);
			}
			mapMeshes[name] = m;
		}
	}

	// make shader and load models to VAO
	{
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

		pShader = pShaderManager->pGetShaderProgramFromFriendlyName("shader01");


		cShaderManager::setCurrentShader(pShader);

		pShader->LoadActiveUniforms();
		//glUseProgram(program);
		view_loc = pShader->getUniformLocID("matView");
		projection_loc = pShader->getUniformLocID("matProjection");
		eyeLocation_loc = pShader->getUniformLocID("eyeLocation");

		for (auto m : mapMeshes)
		{
			if (!pVAOManager->LoadModelIntoVAO(m.first, m.second, program))
			{
				std::cerr << "Failed to load model " << m.first << " to GPU" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
	}

	// Load textures to shader
	{
		pTextureManager = cBasicTextureManager::getTextureManager();
		pTextureManager->SetBasePath("assets/textures");

		std::ifstream texturesjson("assets/textures.json");
		Json::Value textures;
		texturesjson >> textures;
		texturesjson.close();
		for (unsigned i = 0; i < textures["textures"].size(); ++i)
		{
			std::string name = textures["textures"][i].asString();
			std::cout << "Loading texture: " << name << std::endl;
			if (!pTextureManager->Create2DTextureFromBMPFile(name, true)) // NEED TO GENERATE MIP MAPS
			{
				std::cerr << "Failed to load texture " << name << " to GPU" << std::endl;
			}
		}

		// Skyboxes
		for (unsigned i = 0; i < textures["skyboxes"].size(); ++i)
		{
			std::string err;
			std::string name = textures["skyboxes"][i]["name"].asString();
			std::cout << "Loading skybox: " << name << std::endl;
			if (!pTextureManager->CreateCubeTextureFromBMPFiles(
				name,
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
	}

	float ratio;
	int width, height;
	glm::mat4 v, p;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	float fov = 65.0f;

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH);			// Enable depth
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// timing
	float totalTime;
	float lastTime = 0;
	float dt;

	double cursorX, cursorY;
	float lastcursorX = 0, lastcursorY = 0;
	glfwSetCursorPos(window, 0, 0);

	glm::vec2 waterOffset(0.0f);
	bool day_time = true;

	glfwGetCursorPos(window, &cursorX, &cursorY);
	lastcursorX = (float)cursorX;

	// Frame Buffer stuff
	{
		fbo = new cFBO();
		// Usually make this the size of the screen which we can get from opengl
		std::string fboError;
		if (!fbo->init(width, height, fboError))
		{
			std::cout << "FBO init error: " << fboError << std::endl;
		}

		fbo2 = new cFBO();
		// Usually make this the size of the screen which we can get from opengl
		if (!fbo2->init(width, height, fboError))
		{
			std::cout << "FBO init error: " << fboError << std::endl;
		}

		// for resizing
		//fbo->reset();
	}

	cWorld::pDebugRenderer = new cDebugRenderer();
	cWorld::pDebugRenderer->initialize();
	cWorld::debugMode = false;
	pKeyboardManager = new cKeyboardManager();

	camera_inSpace = new cCamera();
	camera_inSpace->position = glm::vec3(0.0f, 0.0f, 0.0f);
	camera_inSpace->yaw = -90;
	camera_inSpace->forward.x = cos(glm::radians(camera_inSpace->yaw)) * cos(glm::radians(camera_inSpace->pitch));
	camera_inSpace->forward.y = sin(glm::radians(camera_inSpace->pitch));
	camera_inSpace->forward.z = sin(glm::radians(camera_inSpace->yaw)) * cos(glm::radians(camera_inSpace->pitch));
	camera_inSpace->forward = glm::normalize(camera_inSpace->forward);
	camera_inSpace->right = glm::normalize(glm::cross(camera_inSpace->forward, camera_inSpace->up));

	camera_inTV = new cCamera();
	camera = camera_inTV;

	// Load physics library
	pPhysicsManager = new cPhysicsManager(physics_library_name);

	openSceneFromFile("assets/scenes/scene1.json");


	camera->forward.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	camera->forward.y = sin(glm::radians(camera->pitch));
	camera->forward.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	camera->forward = glm::normalize(camera->forward);
	camera->right = glm::normalize(glm::cross(camera->forward, camera->up));

	std::vector<cPhysicsGameObject*> balls;

	auto pPhysicsFactory = cPhysicsManager::getFactory();
	auto physWorld = cPhysicsManager::getWorld();

	glfwGetCursorPos(window, &cursorX, &cursorY);
	lastcursorX = cursorX;
	lastcursorY = cursorY;

	std::vector<std::pair<cGameObject*, glm::vec3>> stars;
	{
		for (unsigned i = 0; i < 150; ++i)
		{
			cGameObject* star = new cGameObject();
			star->mesh.meshName = "sphere";
			star->mesh.scale = 0.25f;

			star->transform.position = glm::vec3(0.0f, 0.0f, -50.0f);
			star->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
			star->transform.updateMatricis();

			star->graphics.visible = true;
			star->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			star->graphics.lighting = false;

			float r1 = (rand() / (float)RAND_MAX - 0.5f);
			float r2 = (rand() / (float)RAND_MAX - 0.5f);
			float r3 = (rand() / (float)RAND_MAX);
			stars.push_back(std::make_pair(star, glm::vec3(r1 * 16.0f, r2 * 9.0f, r3 * 50.0f) / 1.0f));
		}

		cGameObject* goConsole = new cGameObject();
		goConsole->mesh.meshName = "console";
		goConsole->mesh.scale = 1.0f;
		goConsole->transform.position = glm::vec3(0.0f);
		goConsole->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goConsole->transform.updateMatricis();
		goConsole->graphics.visible = true;
		goConsole->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goConsole->graphics.lighting = true;
		goConsole->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goConsole->graphics.textures[0].blend = 1.0f;
		goConsole->graphics.textures[0].tiling = 1.0f;
		goConsole->graphics.textures[0].xOffset = 0.0f;
		goConsole->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goConsole);

		cGameObject* goFloor1 = new cGameObject();
		goFloor1->mesh.meshName = "floor";
		goFloor1->mesh.scale = 1.0f;
		goFloor1->transform.position = glm::vec3(-2.5, 0.0f, 0.0f);
		goFloor1->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goFloor1->transform.updateMatricis();
		goFloor1->graphics.visible = true;
		goFloor1->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goFloor1->graphics.lighting = true;
		goFloor1->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goFloor1->graphics.textures[0].blend = 1.0f;
		goFloor1->graphics.textures[0].tiling = 1.0f;
		goFloor1->graphics.textures[0].xOffset = 0.0f;
		goFloor1->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goFloor1);

		cGameObject* goFloor2 = new cGameObject();
		goFloor2->mesh.meshName = "floor";
		goFloor2->mesh.scale = 1.0f;
		goFloor2->transform.position = glm::vec3(2.5, 0.0f, 0.0f);
		goFloor2->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goFloor2->transform.updateMatricis();
		goFloor2->graphics.visible = true;
		goFloor2->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goFloor2->graphics.lighting = true;
		goFloor2->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goFloor2->graphics.textures[0].blend = 1.0f;
		goFloor2->graphics.textures[0].tiling = 1.0f;
		goFloor2->graphics.textures[0].xOffset = 0.0f;
		goFloor2->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goFloor2);

		cGameObject* goFloor3 = new cGameObject();
		goFloor3->mesh.meshName = "floor";
		goFloor3->mesh.scale = 1.0f;
		goFloor3->transform.position = glm::vec3(-2.5, 0.0f, 5.0f);
		goFloor3->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goFloor3->transform.updateMatricis();
		goFloor3->graphics.visible = true;
		goFloor3->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goFloor3->graphics.lighting = true;
		goFloor3->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goFloor3->graphics.textures[0].blend = 1.0f;
		goFloor3->graphics.textures[0].tiling = 1.0f;
		goFloor3->graphics.textures[0].xOffset = 0.0f;
		goFloor3->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goFloor3);

		cGameObject* goFloor4 = new cGameObject();
		goFloor4->mesh.meshName = "floor";
		goFloor4->mesh.scale = 1.0f;
		goFloor4->transform.position = glm::vec3(2.5, 0.0f, 5.0f);
		goFloor4->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goFloor4->transform.updateMatricis();
		goFloor4->graphics.visible = true;
		goFloor4->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goFloor4->graphics.lighting = true;
		goFloor4->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goFloor4->graphics.textures[0].blend = 1.0f;
		goFloor4->graphics.textures[0].tiling = 1.0f;
		goFloor4->graphics.textures[0].xOffset = 0.0f;
		goFloor4->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goFloor4);

		cGameObject* goCorner = new cGameObject();
		goCorner->mesh.meshName = "corner1";
		goCorner->mesh.scale = 1.0f;
		goCorner->transform.position = glm::vec3(-10.0f, 0.0f, 5.0f);
		goCorner->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goCorner->transform.updateMatricis();
		goCorner->graphics.visible = true;
		goCorner->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goCorner->graphics.lighting = true;
		goCorner->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goCorner->graphics.textures[0].blend = 1.0f;
		goCorner->graphics.textures[0].tiling = 1.0f;
		goCorner->graphics.textures[0].xOffset = 0.0f;
		goCorner->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goCorner);

		cGameObject* goCorner2 = new cGameObject();
		goCorner2->mesh.meshName = "corner2";
		goCorner2->mesh.scale = 1.0f;
		goCorner2->transform.position = glm::vec3(5.0f, 0.0f, 5.0f);
		goCorner2->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		goCorner2->transform.updateMatricis();
		goCorner2->graphics.visible = true;
		goCorner2->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goCorner2->graphics.lighting = true;
		goCorner2->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goCorner2->graphics.textures[0].blend = 1.0f;
		goCorner2->graphics.textures[0].tiling = 1.0f;
		goCorner2->graphics.textures[0].xOffset = 0.0f;
		goCorner2->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goCorner2);

		cGameObject* goSeatL = new cGameObject();
		goSeatL->mesh.meshName = "seat";
		goSeatL->mesh.scale = 1.0f;
		goSeatL->transform.position = glm::vec3(-4.0f, 0.0f, 3.5f);
		goSeatL->transform.orientation = glm::quat(0.0f, 0.0f, 1.0f, 0.0f);
		goSeatL->transform.updateMatricis();
		goSeatL->graphics.visible = true;
		goSeatL->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goSeatL->graphics.lighting = true;
		goSeatL->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goSeatL->graphics.textures[0].blend = 1.0f;
		goSeatL->graphics.textures[0].tiling = 1.0f;
		goSeatL->graphics.textures[0].xOffset = 0.0f;
		goSeatL->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goSeatL);

		cGameObject* goSeatR = new cGameObject();
		goSeatR->mesh.meshName = "seat";
		goSeatR->mesh.scale = 1.0f;
		goSeatR->transform.position = glm::vec3(-1.0f, 0.0f, 3.5f);
		goSeatR->transform.orientation = glm::quat(0.0f, 0.0f, 1.0f, 0.0f);
		goSeatR->transform.updateMatricis();
		goSeatR->graphics.visible = true;
		goSeatR->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		goSeatR->graphics.lighting = true;
		goSeatR->graphics.textures[0].fileName = "SpaceInteriors_Texture.bmp";
		goSeatR->graphics.textures[0].blend = 1.0f;
		goSeatR->graphics.textures[0].tiling = 1.0f;
		goSeatR->graphics.textures[0].xOffset = 0.0f;
		goSeatR->graphics.textures[0].yOffset = 0.0f;
		world->addGameObject(goSeatR);

	}

	glActiveTexture(GL_TEXTURE0 + 55);
	glBindTexture(GL_TEXTURE_2D, pTextureManager->getTextureIDFromName("planet.bmp"));
	glUniform1i(pShader->getUniformLocID("planetSamp"), 55);

	// 0 = default
	// 1 = hyperspace
	// 2 = transition out of hyperspace
	unsigned state = 0;

	float planetSize = 0.0f;


	while (!glfwWindowShouldClose(window))
	{
		// Timing
		{
			totalTime = (float)glfwGetTime();
			dt = totalTime - lastTime;
			lastTime = totalTime;

			if (dt >= MAX_DELTA_TIME)
				dt = MAX_DELTA_TIME;

			if (pKeyboardManager->keyDown('G'))
				dt *= 10;
		}

		glfwPollEvents();

		// Camera orientation
		{
			if (shift_pressed)
			{
				camera = camera_inSpace;
			}
			else
			{
				camera = camera_inTV;
			}
				

			glfwGetCursorPos(window, &cursorX, &cursorY);
			camera->yaw += ((float)cursorX - lastcursorX) * camera->sensitivity * dt;
			camera->pitch += (lastcursorY - (float)cursorY) * camera->sensitivity * dt;
			lastcursorX = (float)cursorX;
			lastcursorY = (float)cursorY;

			// Lock pitch
			if (camera->pitch > 89.9f)
				camera->pitch = 89.9f;
			else if (camera->pitch < -89.9f)
				camera->pitch = -89.9f;

			camera->forward.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->forward.y = sin(glm::radians(camera->pitch));
			camera->forward.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->forward = glm::normalize(camera->forward);
			camera->right = glm::normalize(glm::cross(camera->forward, camera->up));
		}

		// keyboard inputs
		{
			if (pKeyboardManager->keyPressed('1'))
				state = 1;
			else if (pKeyboardManager->keyPressed('2'))
				state = 2;

			int xMove = pKeyboardManager->keyDown(GLFW_KEY_A) - pKeyboardManager->keyDown(GLFW_KEY_D);
			int yMove = pKeyboardManager->keyDown(GLFW_KEY_SPACE) - pKeyboardManager->keyDown(GLFW_KEY_C);
			int zMove = pKeyboardManager->keyDown(GLFW_KEY_W) - pKeyboardManager->keyDown(GLFW_KEY_S);

			camera->position += zMove * camera->speed * dt * glm::normalize(glm::cross(camera->up, camera->right));
			camera->position += -xMove * camera->speed * dt * camera->right;
			camera->position += yMove * camera->speed * dt * camera->up;
		}

		// shader uniforms
		{
			// FOV, aspect ratio, near clip, far clip
			p = glm::perspective(glm::radians(fov), ratio, 0.1f, 1000.0f);
			v = glm::lookAt(camera_inTV->position, camera_inTV->position + camera_inTV->forward, camera_inTV->up);

			glUseProgram(program);

			glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
			glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
			glUniform4f(eyeLocation_loc, camera_inTV->position.x, camera_inTV->position.y, camera_inTV->position.z, 1.0f);
			glUniform4f(pShader->getUniformLocID("ambientColour"), ambience[0], ambience[1], ambience[2], ambience[3]);
		}

		// draw Skybox
		{
			// Tie texture
			GLuint texture_ul = pTextureManager->getTextureIDFromName("nighttime");
			if (texture_ul)
			{
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ul);
				glUniform1i(pShader->getUniformLocID("skyboxSamp00"), 10);
			}
			glm::mat4 matWorld(1.0f);
			matWorld *= glm::translate(glm::mat4(1.0f), camera_inTV->position);
			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));
			glUniform4f(pShader->getUniformLocID("diffuseColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), dt, totalTime, 1.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("params2"), 1.0f, 0.0f, 0.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			sModelDrawInfo drawInfo;
			if (pVAOManager->FindDrawInfoByModelName("sphere", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
		}


		// draw starfield to fbo2
		if (state != 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo2->ID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			v = glm::lookAt(camera_inSpace->position, camera_inSpace->position + camera_inSpace->forward, camera_inSpace->up);
			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
			if (planetSize < 1.0f)
			{
				for (auto s : stars)
				{
					s.first->transform.setPosition(s.first->transform.getPosition() + s.second * dt);
					s.first->transform.updateMatricis();
					s.first->graphics.color -= (51.0f - s.second.z) / 50.0f * dt;
					if (s.first->graphics.color.r > (0.65f - planetSize))
						s.first->graphics.color = glm::vec4(0.65f - planetSize);
					s.first->render();
					if (s.first->graphics.color.r <= 0.0f)
					{
						s.first->transform.setPosition(glm::vec3(0.0f, 0.0f, -50.0f));
						s.first->graphics.color = glm::vec4(1.0f);
					}
				}
			}
			// go back to fbo1
			glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);

			v = glm::lookAt(camera_inTV->position, camera_inTV->position + camera_inTV->forward, camera_inTV->up);
			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
		}


		// update
		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			world->vecGameObjects[i]->update(dt, totalTime);
		}
		//physWorld->Update(dt);

		// pre frame, then render
		{
			for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
			{
				world->vecGameObjects[i]->preFrame();
			}
			for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
			{
				world->vecGameObjects[i]->render();
			}
		}

		// draw the moving star field to big box
		if (state != 0)
		{
			glActiveTexture(GL_TEXTURE0 + 40);
			glBindTexture(GL_TEXTURE_2D, fbo2->colourTexture_0_ID);
			glUniform1i(pShader->getUniformLocID("secondPassSamp"), 40);

			glUniform1f(pShader->getUniformLocID("passCount"), 2);
			glUniform4f(pShader->getUniformLocID("planetparams"), 0.0f, 0.0f, 0.0f, 0.0f);
			glm::mat4 matWorld(1.0f);
			matWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 2.0f, -3.0f));
			matWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(16.0f, 9.0f, 1.0f));
			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));

			sModelDrawInfo drawInfo;
			if (pVAOManager->FindDrawInfoByModelName("cube", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			if (state == 2)
			{
				planetSize += dt * 0.1f;
				if (planetSize > 1.0f)
					planetSize = 1.0f;
				glUniform4f(pShader->getUniformLocID("planetparams"), planetSize, 0.0, 0.0, 0.0);

				matWorld = glm::mat4(1.0f);
				matWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(-2.5f, 2.0f, -2.99f));
				matWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(3.0f * planetSize, 3.0f * planetSize, 1.0f));
				glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
				glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));

				if (pVAOManager->FindDrawInfoByModelName("cube", drawInfo))
				{
					glBindVertexArray(drawInfo.VAO_ID);
					glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}

			glUniform1f(pShader->getUniformLocID("passCount"), 1);
		}

		// render fbo to tri
		{
			glUseProgram(program);

			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// switch off fbo
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 3. use the FBO colour texture as the texture on the tri
			glActiveTexture(GL_TEXTURE0 + 40);
			glBindTexture(GL_TEXTURE_2D, fbo->colourTexture_0_ID);
			glUniform1i(pShader->getUniformLocID("secondPassSamp"), 40);

			glUniform1f(pShader->getUniformLocID("passCount"), 3);

			glUniform4f(pShader->getUniformLocID("diffuseColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), dt, totalTime, 0.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

			glm::mat4 matWorld(1.0f);
			matWorld *= glm::translate(glm::mat4(1.0f), camera_inTV->position - camera_inTV->forward * -2.0f);
			matWorld *= glm::mat4(glm::quatLookAt(camera_inTV->forward, glm::normalize(glm::cross(camera_inTV->forward, camera_inTV->right))));
			matWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f));
			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));

			// 4. draw a single object, (tri or quad)
			sModelDrawInfo drawInfo;
			if (pVAOManager->FindDrawInfoByModelName("triangle", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			glUniform1f(pShader->getUniformLocID("passCount"), 1);
		}



		glfwSwapBuffers(window); // Draws to screen

		// window title
		//if (world->vecGameObjects.size() && world->vecLights.size())
		{
			std::ostringstream windowTitle;
			windowTitle << std::fixed << std::setprecision(3)
				<< "{" << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << "} "
				<< "{" << camera->forward.x << ", " << camera->forward.y << ", " << camera->forward.z << "} "
				<< "dt: " << dt;

			//if (selectedObject < world->vecGameObjects.size())
			//{
			//	windowTitle << "Obj[" << selectedObject << "]: \"" << world->vecGameObjects[selectedObject]->name << "\" ";
			//}
			//if (shift_pressed)
			//{
			//	if (selectedLight < world->vecLights.size())
			//	{
			//		windowTitle << "Light[" << selectedLight << "]";
			//	}
			//}
			glfwSetWindowTitle(window, windowTitle.str().c_str());
		}

		world->doDeferredActions();
		cKeyboardManager::update();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	// Delete everything
	{
		for (auto g : world->vecGameObjects)
			delete g;

		for (auto l : world->vecLights)
			delete l;

		for (auto m : mapMeshes)
			delete m.second;

		for (auto s : stars)
			delete s.first;

		delete pShaderManager;
		delete pModelLoader;
		delete pVAOManager;
		delete pKeyboardManager;
		//delete pTextureManager;
		delete cWorld::pDebugRenderer;

		delete fbo;
		delete fbo2;

		delete pPhysicsManager;
	}

	return 0;
}
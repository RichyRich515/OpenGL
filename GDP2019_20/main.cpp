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

	//Json::Value ambienceobj = world_node["ambience"];
	//for (unsigned i = 0; i < 4; ++i) // vec 4s
	//{
	//	ambience[i] = ambienceobj[i].asFloat();
	//}

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
		if (!fbo->init(width, height, fboError))
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
	camera_inTV = new cCamera();
	camera_inTV->position = glm::vec3(0.0f, 5.0f, 30.0f);
	camera_inTV->yaw = -90;
	camera_inTV->forward.x = cos(glm::radians(camera_inTV->yaw)) * cos(glm::radians(camera_inTV->pitch));
	camera_inTV->forward.y = sin(glm::radians(camera_inTV->pitch));
	camera_inTV->forward.z = sin(glm::radians(camera_inTV->yaw)) * cos(glm::radians(camera_inTV->pitch));
	camera_inTV->forward = glm::normalize(camera_inTV->forward);
	camera_inTV->right = glm::normalize(glm::cross(camera_inTV->forward, camera_inTV->up));

	camera_inSpace = new cCamera();
	camera = camera_inSpace;

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

	cGameObject* goTVBody = new cGameObject();
	goTVBody->mesh.meshName = "TV_body";
	goTVBody->mesh.scale = 1.0f;
	goTVBody->transform.position = glm::vec3(0.0f);
	goTVBody->transform.orientation = glm::quat(0.707f, -0.707f, 0.0f, 0.0f);
	goTVBody->transform.updateMatricis();
	goTVBody->graphics.visible = true;
	goTVBody->graphics.color = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
	goTVBody->graphics.lighting = true;


	cGameObject* goRefl = new cGameObject();
	goRefl->mesh.meshName = "crystal";
	goRefl->mesh.scale = 2.0f;
	goRefl->transform.position = glm::vec3(5.0f, 0.0f, 15.0f);
	goRefl->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	goRefl->transform.updateMatricis();
	goRefl->graphics.visible = true;
	goRefl->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	goRefl->graphics.lighting = true;
	goRefl->graphics.reflects = true;
	world->addGameObject(goRefl);

	cGameObject* goRefr = new cGameObject();
	goRefr->mesh.meshName = "crystal";
	goRefr->mesh.scale = 2.0f;
	goRefr->transform.position = glm::vec3(-5.0f, 0.0f, 15.0f);
	goRefr->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	goRefr->transform.updateMatricis();
	goRefr->graphics.visible = true;
	goRefr->graphics.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	goRefr->graphics.lighting = true;
	goRefr->graphics.refracts = true;
	world->addGameObject(goRefr);

	glActiveTexture(GL_TEXTURE0 + 55);
	glBindTexture(GL_TEXTURE_2D, pTextureManager->getTextureIDFromName("noise.bmp"));
	glUniform1i(pShader->getUniformLocID("noiseSamp"), 55);

	glActiveTexture(GL_TEXTURE0 + 56);
	glBindTexture(GL_TEXTURE_2D, pTextureManager->getTextureIDFromName("scope.bmp"));
	glUniform1i(pShader->getUniformLocID("scopeSamp"), 56);

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

			glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);

			glUseProgram(program);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
			glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
			glUniform4f(eyeLocation_loc, camera_inTV->position.x, camera_inTV->position.y, camera_inTV->position.z, 1.0f);
			glUniform4f(pShader->getUniformLocID("ambientColour"), ambience[0], ambience[1], ambience[2], ambience[3]);

			waterOffset.s += rand() / (float)RAND_MAX * 10.0f;
			waterOffset.t += rand() / (float)RAND_MAX * 10.0f;
			glUniform2f(pShader->getUniformLocID("waterOffset"), waterOffset.x, waterOffset.y);
		}

		// draw Skybox
		{
			// Tie texture
			GLuint texture_ul = pTextureManager->getTextureIDFromName("daytime");
			if (texture_ul)
			{
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ul);
				glUniform1i(pShader->getUniformLocID("skyboxSamp00"), 10);
			}
			texture_ul = pTextureManager->getTextureIDFromName("nighttime");
			if (texture_ul)
			{
				glActiveTexture(GL_TEXTURE11);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ul);
				glUniform1i(pShader->getUniformLocID("skyboxSamp01"), 11);
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
			glUniform1i(pShader->getUniformLocID("daytime"), day_time);
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

		// update
		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			world->vecGameObjects[i]->update(dt, totalTime);
		}
		goTVBody->update(dt, totalTime);
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

		// draw debug
		if (cWorld::debugMode)
		{
			cWorld::pDebugRenderer->addLine(glm::vec3(-200.0f, 0.0f, 0.0f), glm::vec3(200.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f);
			cWorld::pDebugRenderer->addLine(glm::vec3(0.0f, -200.0f, 0.0f), glm::vec3(0.0f, 200.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);
			cWorld::pDebugRenderer->addLine(glm::vec3(0.0f, 0.0f, -200.0f), glm::vec3(0.0f, 0.0f, 200.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.0f);

			cWorld::pDebugRenderer->RenderDebugObjects(v, p, dt);
		}

		// Draw frame buffer to TV screen
		{
			glUseProgram(program);

			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// switch to fbo2
			glBindFramebuffer(GL_FRAMEBUFFER, fbo2->ID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUniform1f(pShader->getUniformLocID("passCount"), 1);

			v = glm::lookAt(camera_inSpace->position, camera_inSpace->position + camera_inSpace->forward, camera_inSpace->up);
			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));

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
				texture_ul = pTextureManager->getTextureIDFromName("daytime");
				if (texture_ul)
				{
					glActiveTexture(GL_TEXTURE11);
					glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ul);
					glUniform1i(pShader->getUniformLocID("skyboxSamp01"), 11);
				}
				glm::mat4 matWorld(1.0f);
				matWorld *= glm::translate(glm::mat4(1.0f), camera_inSpace->position);
				glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
				glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));
				glUniform4f(pShader->getUniformLocID("diffuseColour"), 0.0f, 0.0f, 0.0f, 1.0f);
				glUniform4f(pShader->getUniformLocID("specularColour"), 0.0f, 0.0f, 0.0f, 1.0f);
				glUniform4f(pShader->getUniformLocID("params1"), dt, totalTime, 1.0f, 0.0f);
				glUniform4f(pShader->getUniformLocID("params2"), 1.0f, 0.0f, 0.0f, 0.0f);
				glUniform4f(pShader->getUniformLocID("heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);
				glUniform1i(pShader->getUniformLocID("daytime"), day_time);
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

			goTVBody->render();
			// 3. use the FBO colour texture as the texture on the quad
			glActiveTexture(GL_TEXTURE0 + 40);
			glBindTexture(GL_TEXTURE_2D, fbo->colourTexture_0_ID);
			glUniform1i(pShader->getUniformLocID("secondPassSamp"), 40);
			glUniform1f(pShader->getUniformLocID("passCount"), 2);

			glUniform4f(pShader->getUniformLocID("diffuseColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), dt, totalTime, 0.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

			glm::mat4 matWorld(1.0f);
			matWorld *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
			matWorld *= glm::mat4(glm::quat(0.707f, -0.707f, 0.0f, 0.0f));
			matWorld *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));

			// 4. draw a single object, (tri or quad)
			sModelDrawInfo drawInfo;
			if (pVAOManager->FindDrawInfoByModelName("TV_screen", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}

			glUniform1f(pShader->getUniformLocID("passCount"), 1);
		}

		

		// render fbo2 to tri
		{
			glUseProgram(program);

			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// switch to fbo2
			glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 3. use the FBO colour texture as the texture on the quad
			glActiveTexture(GL_TEXTURE0 + 41);
			glBindTexture(GL_TEXTURE_2D, fbo2->colourTexture_0_ID);
			glUniform1i(pShader->getUniformLocID("secondPassSamp"), 41);

			glUniform1f(pShader->getUniformLocID("passCount"), 3);

			glUniform4f(pShader->getUniformLocID("diffuseColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), dt, totalTime, 0.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

			glm::mat4 matWorld(1.0f);
			matWorld *= glm::translate(glm::mat4(1.0f), camera_inSpace->position - camera_inSpace->forward * -2.0f);
			matWorld *= glm::mat4(glm::quatLookAt(camera_inSpace->forward, glm::normalize(glm::cross(camera_inSpace->forward, camera_inSpace->right))));
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

		delete pShaderManager;
		delete pModelLoader;
		delete pVAOManager;
		delete pKeyboardManager;
		//delete pTextureManager;
		delete cWorld::pDebugRenderer;
		delete goTVBody;

		delete fbo;
		delete fbo2;

		delete pPhysicsManager;
	}

	return 0;
}
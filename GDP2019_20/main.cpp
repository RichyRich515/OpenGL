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

#include "cCamera.hpp"
#include "cMesh.hpp"
#include "cModelLoader.hpp"
#include "Texture/cBasicTextureManager.h"
#include "cVAOManager.hpp"
#include "cShaderManager.hpp"
#include "cGameObject.hpp"
#include "cLight.hpp"
#include "Physics.hpp"
#include "cKeyboardManager.hpp"

#include "iGameObjectFactory.hpp"
#include "cFactoryManager.hpp"

#include "DebugRenderer/cDebugRenderer.h"

#include "cWorld.hpp"

#include "cParticleEmitter.hpp"

#include "cFBO.h"

#include <iPhysicsFactory.h>


#include <Windows.h>

constexpr char physics_interface_factory_func_name[] = "MakePhysicsFactory";
typedef nPhysics::iPhysicsFactory* (func_createPhysicsFactory)();

#define MY_PHYSICS

#ifdef MY_PHYSICS
constexpr char physics_library_name[21] = "MyPhysicsWrapper.dll";
#elif
constexpr char physics_library_name[] = "BulletWrapper.dll";
#endif

nPhysics::iPhysicsFactory* pPhysicsFactory;

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
void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager, float dt, float tt);

constexpr float MAX_DELTA_TIME = 0.017f;

cCamera* camera;


// TODO: Get this outta global space
cShaderManager::cShaderProgram* pShader;
GLuint program = 0;

std::map<std::string, cMesh*> mapMeshes;

cFBO* fbo = nullptr;

// factory for any game object
iGameObjectFactory* pGameObjectFactory;
cBasicTextureManager* pTextureManager;

int selectedObject = 0;
int selectedLight = 0;

// Max lights from the shader
constexpr unsigned MAX_LIGHTS = 20;

cWorld* world = cWorld::getWorld();

glm::vec3 gravity;
glm::vec4 ambience;


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
	Json::Value camera_node = root["camera"];
	Json::Value lights = root["lights"];
	Json::Value gameObjects = root["gameObjects"];

	for (unsigned i = 0; i < 3; ++i)
	{
		camera->position[i] = camera_node["cameraEye"][i].asFloat();
		gravity[i] = world_node["gravity"][i].asFloat();
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
		cGameObject* go = pGameObjectFactory->createFromJSON(gameObjects[i], mapMeshes);
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

		// NUB

		const int NUB_NAME_BUFFERSIZE = 1000;
		char NUB_name[NUB_NAME_BUFFERSIZE] = { 0, };
		int charactersGLWrote = 0;
		glGetActiveUniformBlockName(pShader->ID, 0, NUB_NAME_BUFFERSIZE, &charactersGLWrote, NUB_name);
		// TODO: more nub stuff

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
		pTextureManager = new cBasicTextureManager();
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

	cGameObject* debugSphere = new cGameObject("debugsphere");
	debugSphere->meshName = "sphere";
	debugSphere->inverseMass = 0.0f;
	debugSphere->wireFrame = true;
	debugSphere->lighting = true;

	cGameObject* triangle = new cGameObject("triangle");
	triangle->meshName = "triangle";
	triangle->scale = 100.0f;

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
	//glEnable(GL_MULTISAMPLE);
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

		// for resizing
		//fbo->reset();

	}

	cWorld::pDebugRenderer = new cDebugRenderer();
	cWorld::pDebugRenderer->initialize();
	cWorld::debugMode = false;
	pKeyboardManager = new cKeyboardManager();
	camera = new cCamera();
	
	openSceneFromFile("scene1.json");

	// Load physics library
	HMODULE hModule = NULL;
	{
		hModule = LoadLibraryA(physics_library_name);
		if (!hModule)
		{
			std::cout << "Error loading " << physics_library_name << std::endl;
			return EXIT_FAILURE;
		}
		// make a Physics factory
		pPhysicsFactory = ((func_createPhysicsFactory*)GetProcAddress(hModule, physics_interface_factory_func_name))();
	}

	auto physWorld = pPhysicsFactory->CreateWorld();
	nPhysics::sBallDef def = nPhysics::sBallDef{ 1.0f, 1.0f, glm::vec3(-200.0f, 65.0f, -50.0f) };
	auto ball1 = pPhysicsFactory->CreateBall(def);

	def.Position.z += 10.0f;
	def.Position.y += 5.0f;
	auto ball2 = pPhysicsFactory->CreateBall(def);

	physWorld->AddComponent(ball1);
	physWorld->AddComponent(ball2);
	glm::vec3 n = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
	physWorld->AddComponent(pPhysicsFactory->CreatePlane(nPhysics::sPlaneDef{ glm::dot(glm::vec3(0.0f, 30.0f, 0.0f), n), n }));

	cWorld::debugMode = true;
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

		// Camera orientation movement
		{
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

			// debugging camera
			camera->forward.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->forward.y = sin(glm::radians(camera->pitch));
			camera->forward.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->forward = glm::normalize(camera->forward);
			camera->right = glm::normalize(glm::cross(camera->forward, camera->up));
		}

		// keyboard inputs
		{
			if (pKeyboardManager->keyPressed(GLFW_KEY_GRAVE_ACCENT))
			{
				cWorld::debugMode = !cWorld::debugMode;
			}
			if (pKeyboardManager->keyPressed(GLFW_KEY_V))
			{
				day_time = !day_time;
				if (day_time)
				{
					world->vecLights[0]->diffuse = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
				}
				else
				{
					world->vecLights[0]->diffuse = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);
				}
				world->vecLights[0]->updateShaderUniforms();
			}

			if (pKeyboardManager->keyPressed(GLFW_KEY_F1))
			{
				std::ostringstream fileName;
				fileName << "scene_" << time(NULL) << ".json";
				writeSceneToFile(fileName.str());
			}

			int xMove = pKeyboardManager->keyDown(GLFW_KEY_A) - pKeyboardManager->keyDown(GLFW_KEY_D);
			int yMove = pKeyboardManager->keyDown(GLFW_KEY_SPACE) - pKeyboardManager->keyDown(GLFW_KEY_C);
			int zMove = pKeyboardManager->keyDown(GLFW_KEY_W) - pKeyboardManager->keyDown(GLFW_KEY_S);

			int xRot = pKeyboardManager->keyDown(GLFW_KEY_I) - pKeyboardManager->keyDown(GLFW_KEY_K);
			int yRot = pKeyboardManager->keyDown(GLFW_KEY_U) - pKeyboardManager->keyDown(GLFW_KEY_O);
			int zRot = pKeyboardManager->keyDown(GLFW_KEY_J) - pKeyboardManager->keyDown(GLFW_KEY_L);

			int scaleFactor = pKeyboardManager->keyDown(GLFW_KEY_R) - pKeyboardManager->keyDown(GLFW_KEY_F);

			if (pKeyboardManager->keyPressed(GLFW_KEY_V))
			{
				if (shift_pressed)
				{
					if (world->vecLights.size())
						world->vecLights[selectedLight]->param2.x = !world->vecLights[selectedLight]->param2.x;
				}
				else
				{
					if (world->vecGameObjects.size())
						world->vecGameObjects[selectedObject]->visible = !world->vecGameObjects[selectedObject]->visible;
				}
			}
			if (pKeyboardManager->keyPressed(GLFW_KEY_B))
			{
				if (world->vecGameObjects.size())
					world->vecGameObjects[selectedObject]->wireFrame = !world->vecGameObjects[selectedObject]->wireFrame;
			}

			if (pKeyboardManager->keyPressed(GLFW_KEY_PERIOD))
			{
				if (shift_pressed)
				{
					++selectedLight;
					if (selectedLight >= world->vecLights.size())
						selectedLight = 0;
				}
				else
				{
					++selectedObject;
					if (selectedObject >= world->vecGameObjects.size())
						selectedObject = 0;
				}
			}
			else if (pKeyboardManager->keyPressed(GLFW_KEY_COMMA))
			{
				if (shift_pressed)
				{
					--selectedLight;
					if (selectedLight < 0)
						selectedLight = (int)world->vecLights.size() - 1;
				}
				else
				{
					--selectedObject;
					if (selectedObject < 0)
						selectedObject = (int)world->vecGameObjects.size() - 1;
				}
			}

			if (ctrl_pressed)
			{
				if (world->vecGameObjects.size())
				{
					glm::vec3 velocity = dt * 3.0f * glm::vec3(xMove, yMove, zMove);
					glm::vec3 rotation = dt * 0.5f * glm::vec3(xRot, yRot, zRot);
					world->vecGameObjects[selectedObject]->translate(velocity);
					world->vecGameObjects[selectedObject]->scale *= (scaleFactor ? (scaleFactor * 0.01f + 1.0f) : 1.0f); // change by 1%
					world->vecGameObjects[selectedObject]->rotate(rotation);
				}
			}
			else if (shift_pressed)
			{
				if (world->vecLights.size())
				{
					float speed = 3.0f;
					// Move light if shift pressed
					world->vecLights[selectedLight]->position.x += xMove * speed * dt;
					world->vecLights[selectedLight]->position.y += yMove * speed * dt;
					world->vecLights[selectedLight]->position.z += zMove * speed * dt;
					world->vecLights[selectedLight]->atten.y *= (scaleFactor ? (scaleFactor * 0.01f + 1.0f) : 1.0f); // Linear

					world->vecLights[selectedLight]->updateShaderUniforms();

					debugSphere->setPosition(world->vecLights[selectedLight]->position);
					debugSphere->scale = 1.0f;//0.1f / world->vecLights[selectedLight]->atten.y;
					debugSphere->color = world->vecLights[selectedLight]->diffuse;
					debugSphere->wireFrame = true;
					debugSphere->visible = true;
					debugSphere->lighting = false;
					debugSphere->updateMatricis();
					// Draw light sphere if shift pressed
					drawObject(debugSphere, program, pVAOManager, dt, totalTime);
				}
			}
			else
			{
				camera->position += zMove * camera->speed * dt * glm::normalize(glm::cross(camera->up, camera->right));
				camera->position += -xMove * camera->speed * dt * camera->right;
				camera->position += yMove * camera->speed * dt * camera->up;
			}
		}

		// shader uniforms
		{
			// FOV, aspect ratio, near clip, far clip
			p = glm::perspective(glm::radians(fov), ratio, 0.1f, 1000.0f);
			v = glm::lookAt(camera->position, camera->position + camera->forward, camera->up);

			glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);

			glUseProgram(program);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
			glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
			glUniform4f(eyeLocation_loc, camera->position.x, camera->position.y, camera->position.z, 1.0f);
			glUniform4f(pShader->getUniformLocID("ambientColour"), ambience[0], ambience[1], ambience[2], ambience[3]);

			waterOffset.s += 0.1f * dt;
			waterOffset.t += 0.017f * dt;
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
			debugSphere->scale = 1.0f;
			debugSphere->setPosition(camera->position);
			debugSphere->wireFrame = false;
			debugSphere->lighting = false;
			debugSphere->color = glm::vec4(1.0f);
			debugSphere->visible = true;
			debugSphere->updateMatricis();
			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(debugSphere->matWorld));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(debugSphere->inverseTransposeMatWorld));
			glUniform4f(pShader->getUniformLocID("diffuseColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), dt, totalTime, 1.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("params2"), 1.0f, 0.0f, 0.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);
			glUniform1i(pShader->getUniformLocID("daytime"), day_time);
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

		physWorld->Update(dt);

		// update and draw objects
		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			world->vecGameObjects[i]->update(dt, totalTime);
			world->vecGameObjects[i]->updateMatricis();

			if (!world->vecGameObjects[i]->visible)
				continue;
			drawObject(world->vecGameObjects[i], program, pVAOManager, dt, totalTime);
		}

		// draw debug
		if (cWorld::debugMode)
		{
			glm::mat4 mat(1.0f);
			debugSphere->wireFrame = true;
			debugSphere->scale = 2.0f;

			ball1->GetTransform(mat);
			debugSphere->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			debugSphere->setPosition(mat[3]);
			debugSphere->updateMatricis();
			drawObject(debugSphere, program, pVAOManager, dt, totalTime);

			ball2->GetTransform(mat);
			debugSphere->color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			debugSphere->setPosition(mat[3]);
			debugSphere->updateMatricis();
			drawObject(debugSphere, program, pVAOManager, dt, totalTime);
			std::cout << std::to_string(mat[3]) << std::endl;
			cWorld::pDebugRenderer->RenderDebugObjects(v, p, dt);
		}

		// Draw frame buffer to screen
		{
			glUseProgram(program);
			// whole scene is drawn to buffer
			// 1. disable the FBO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 2. clear actual screen buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 3. use the FBO colour texture as the texture on the quad
			glActiveTexture(GL_TEXTURE0 + 40);
			glBindTexture(GL_TEXTURE_2D, fbo->colourTexture_0_ID);
			glUniform1i(pShader->getUniformLocID("secondPassSamp"), 40);
			glUniform1f(pShader->getUniformLocID("passCount"), 2);

			// 4. draw a single object, (tri or quad)
			triangle->setPosition(camera->position - camera->forward * -2.0f);
			triangle->setOrientation(glm::quatLookAt(camera->forward, glm::normalize(glm::cross(camera->forward, camera->right))));
			triangle->updateMatricis();
			drawObject(triangle, program, pVAOManager, dt, totalTime);
			glUniform1f(pShader->getUniformLocID("passCount"), 1);

		}


		glfwSwapBuffers(window); // Draws to screen

		// window title
		if (world->vecGameObjects.size() && world->vecLights.size())
		{
			std::ostringstream windowTitle;
			windowTitle << std::fixed << std::setprecision(2)
				<< "{" << camera->position.x << ", " << camera->position.y << ", " << camera->position.z << "} "
				<< "{" << camera->forward.x << ", " << camera->forward.y << ", " << camera->forward.z << "} ";

			if (selectedObject < world->vecGameObjects.size())
			{
				windowTitle << "Obj[" << selectedObject << "]: \"" << world->vecGameObjects[selectedObject]->name << "\" ";
			}
			if (shift_pressed)
			{
				if (selectedLight < world->vecLights.size())
				{
					windowTitle << "Light[" << selectedLight << "]";
				}
			}
			glfwSetWindowTitle(window, windowTitle.str().c_str());
		}

		world->doDeferredActions();
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
		delete pTextureManager;
		delete cWorld::pDebugRenderer;

		delete debugSphere;
		delete pPhysicsFactory;

		if (hModule)
		{
			FreeLibrary(hModule);
			hModule = NULL;
		}
	}

	return 0;
}


// Draw an object
void drawObject(cGameObject* go, GLuint shader, cVAOManager* pVAOManager, float dt, float tt)
{
	// Tie textures
	{
		GLuint texture_ul = pTextureManager->getTextureIDFromName(go->textures[0].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp00"), 0);
			glUniform4f(pShader->getUniformLocID("textparams00"),
				go->textures[0].xOffset,
				go->textures[0].yOffset,
				go->textures[0].blend,
				go->textures[0].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams00"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
		texture_ul = pTextureManager->getTextureIDFromName(go->textures[1].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp01"), 1);
			glUniform4f(pShader->getUniformLocID("textparams01"),
				go->textures[1].xOffset,
				go->textures[1].yOffset,
				go->textures[1].blend,
				go->textures[1].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams01"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
		texture_ul = pTextureManager->getTextureIDFromName(go->textures[2].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp02"), 2);
			glUniform4f(pShader->getUniformLocID("textparams02"),
				go->textures[2].xOffset,
				go->textures[2].yOffset,
				go->textures[2].blend,
				go->textures[2].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams02"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
		texture_ul = pTextureManager->getTextureIDFromName(go->textures[3].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp03"), 3);
			glUniform4f(pShader->getUniformLocID("textparams03"),
				go->textures[3].xOffset,
				go->textures[3].yOffset,
				go->textures[3].blend,
				go->textures[3].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams03"), 0.0f, 0.0f, 0.0f, 0.0f);
		}

		// Height map
		texture_ul = pTextureManager->getTextureIDFromName(go->heightmap.fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 40);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("heightSamp"), 40);
			glUniform4f(pShader->getUniformLocID("heightparams"),
				go->heightmap.xOffset,
				go->heightmap.yOffset,
				go->heightmap.blend,
				go->heightmap.tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);
		}

		// discard map
		texture_ul = pTextureManager->getTextureIDFromName(go->discardmap.fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 50);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("discardSamp"), 50);
			glUniform4f(pShader->getUniformLocID("discardparams"),
				go->discardmap.xOffset,
				go->discardmap.yOffset,
				go->discardmap.blend,
				go->discardmap.tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("discardparams"), 0.0f, 0.0f, 0.0f, 0.0f);
		}

	}

	glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(go->matWorld));
	glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(go->inverseTransposeMatWorld));
	glUniform4f(pShader->getUniformLocID("diffuseColour"), go->color.r, go->color.g, go->color.b, go->color.a);
	glUniform4f(pShader->getUniformLocID("specularColour"), go->specular.r, go->specular.g, go->specular.b, go->specular.a);
	glUniform4f(pShader->getUniformLocID("params1"), dt, tt, (float)go->lighting, 0.0f);
	glUniform4f(pShader->getUniformLocID("params2"),
		0.0f,
		go->name == "terrain" ? 1.0f : 0.0f,
		go->name == "ocean" || go->name == "sand_floor" ? 1.0f : 0.0f,
		0.0f);

	if (go->wireFrame)
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		// glCullFace
		//glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
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
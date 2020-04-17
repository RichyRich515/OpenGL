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
#include <functional>
#include <thread>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <json/json.h>
#include "JsonHelper.hpp"

#include "cModelLoader.hpp"
#include "cVAOManager.hpp"
#include "cShaderManager.hpp"
#include "Texture/cBasicTextureManager.h"
#include "cKeyboardManager.hpp"
#include "cPhysicsManager.hpp"
#include "cFactoryManager.hpp"

#include "iGameObjectFactory.hpp"

#include "DebugRenderer/cDebugRenderer.h"
#include "cFBO_deferred.hpp"
#include "cMesh.hpp"

#include "cWorld.hpp"
#include "cCamera.hpp"
#include "iGameObject.hpp"
#include "cGameObject.hpp"
#include "cPhysicsGameObject.hpp"
#include "cBulletGameObject.hpp"
#include "cLight.hpp"

constexpr char physics_library_name[25] = "BulletPhysicsWrapper.dll";

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

constexpr float MAX_DELTA_TIME = 0.017f;

cCamera* camera;

// TODO: Get this outta global space
cShaderManager::cShaderProgram* pShader;
GLuint program = 0;

std::map<std::string, cMesh*> mapMeshes;

cFBO_deferred* fbo = nullptr;

// multithreaded texture loader
std::vector<std::thread> vec_threadPool;
std::vector<bool> vec_threadsDone;
std::vector<std::string> vec_texturesToLoadFromFile;
std::vector<CTextureFromBMP*> vec_texturesToLoadToGPU;
std::mutex texture_mutex;

// factory for any game object
iGameObjectFactory* pGameObjectFactory;
cBasicTextureManager* pTextureManager;
cKeyboardManager* pKeyboardManager = NULL;
bool ctrl_pressed = false, shift_pressed = false;
bool left_mouse_down = false;

// Max lights from the shader
constexpr unsigned MAX_LIGHTS = 20;

cWorld* world = cWorld::getWorld();

glm::vec3 gravity;
glm::vec4 ambience = glm::vec4(0.76f);

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
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		left_mouse_down = true;
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		left_mouse_down = false;
}

void Threaded_LoadTexturesFromFiles(unsigned thread_id)
{
	while (!vec_texturesToLoadFromFile.empty())
	{
		texture_mutex.lock();
		std::string name = (*vec_texturesToLoadFromFile.begin());
		vec_texturesToLoadFromFile.erase(vec_texturesToLoadFromFile.begin());
		texture_mutex.unlock();

		//std::cout << "Loading texture: " << name << std::endl;
		CTextureFromBMP* texture = pTextureManager->Create2DTextureFromBMPFile(name);
		if (texture == nullptr) 
		{
			std::cerr << "Failed to load texture " << name << " from file" << std::endl;
		}
		else
		{
			texture_mutex.lock();
			vec_texturesToLoadToGPU.push_back(texture);
			texture_mutex.unlock();
		}
	}
	vec_threadsDone[thread_id] = true;
	std::cout << "Thread done" << std::endl;
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
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		window = glfwCreateWindow(1920, 1080, "", NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		glfwSetKeyCallback(window, key_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable cursor and lock to window
		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glfwSwapInterval(0); // Same idea as vsync, setting this to 0 would result in unlocked framerate and potentially cause screen tearing
		if (!gladLoadGL())
		{
			std::cerr << "Failed to load glad" << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		// window title
		std::ostringstream windowTitle;
		windowTitle << "OpenGL" << std::endl;

		glfwSetWindowTitle(window, windowTitle.str().c_str());
	}

	// imgui setup
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 420");
	}

	cModelLoader* pModelLoader = new cModelLoader();
	cVAOManager* pVAOManager = new cVAOManager();
	cVAOManager::setCurrentVAOManager(pVAOManager);
	cShaderManager* pShaderManager = new cShaderManager();
	camera = new cCamera();
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

		// Load a default texture
		if (!pTextureManager->CreateDefault2DTextureFromBMPFileAndLoadToGPU("default.bmp", true)) // NEED TO GENERATE MIP MAPS
		{
			std::cerr << "Failed to load texture default.bmp from file to GPU" << std::endl;
		}

		std::ifstream texturesjson("assets/textures.json");
		Json::Value textures;
		texturesjson >> textures;
		texturesjson.close();
		for (unsigned i = 0; i < textures["textures"].size(); ++i)
		{
			// put textures into queue
			std::string name = textures["textures"][i].asString();
			vec_texturesToLoadFromFile.push_back(name);
		}

		// Setup threading stuff for texture loading
		{
			// Query thread count, want a minimum 4 threads regardless.
			unsigned numThreads = std::thread::hardware_concurrency();
			if (numThreads < 4)
				numThreads = 4;

			for (unsigned i = 0; i < numThreads; ++i)
			{
				vec_threadsDone.push_back(false);
				vec_threadPool.push_back(std::thread(Threaded_LoadTexturesFromFiles, i));
				vec_threadPool[i].detach();
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
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// timing
	float tt;
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
		fbo = new cFBO_deferred();
		// Usually make this the size of the screen which we can get from opengl
		std::string fboError;
		if (!fbo->init(width, height, fboError))
		{
			std::cout << "FBO init error: " << fboError << std::endl;
		}

		fbo->clearBuffers();
		// for resizing
		//fbo->reset();
	}

	cWorld::pDebugRenderer = new cDebugRenderer();
	cWorld::pDebugRenderer->initialize();
	cWorld::debugMode = false;
	pKeyboardManager = new cKeyboardManager();

	// Load physics library
	pPhysicsManager = new cPhysicsManager(physics_library_name);

	openSceneFromFile("assets/scenes/scene1.json");

	// setup textured cubes
	{
		unsigned count = 1;
		for (unsigned y = 0; y < 10; ++y)
		{
			for (unsigned x = 0; x < 21; ++x)
			{
				cGameObject* cube = new cGameObject();
				cube->graphics.visible = true;
				cube->graphics.lighting = true;
				cube->graphics.textures[0].fileName = "texture (" + std::to_string(count) + ").bmp";
				cube->graphics.textures[0].blend = 1.0f;
				cube->graphics.textures[0].tiling = 1.0f;

				cube->mesh.meshName = "cube";
				cube->mesh.scale = 5.0f;

				cube->transform.position.x = 100.0f + x * -10.0f;
				cube->transform.position.y = 90.0f + y * -10.0f;
				cube->transform.position.z = 120.0f;
				cube->transform.orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
				cube->transform.updateMatricis();
				world->addGameObject(cube);
				++count;
			}
		}
	}

	camera->forward.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	camera->forward.y = sin(glm::radians(camera->pitch));
	camera->forward.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
	camera->forward = glm::normalize(camera->forward);
	camera->right = glm::normalize(glm::cross(camera->forward, camera->up));

	auto pPhysicsFactory = cPhysicsManager::getFactory();
	auto physWorld = cPhysicsManager::getWorld();

	nPhysics::sCharacterDef def;
	def.Height = 7.5f;
	def.Radius = 2.0f;
	def.Mass = 20.0f;
	def.Position = glm::vec3(0, 10, -60);
	def.JumpSpeed = 20.0f;
	nPhysics::iCharacterComponent* physicscharacter = pPhysicsFactory->CreateCharacter(def);

	pPhysicsManager->getWorld()->AddComponent(physicscharacter);

	cGameObject* character = new cGameObject();
	character->graphics.visible = true;
	character->graphics.lighting = true;
	character->graphics.color = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);

	character->mesh.meshName = "sphere";
	character->mesh.scale = 4.0f;

	world->addGameObject(character);

	glfwGetCursorPos(window, &cursorX, &cursorY);
	lastcursorX = cursorX;
	lastcursorY = cursorY;
	
	bool done_loading = false;

	// Wait for threads to load in all the textures
	do
	{
		done_loading = true;
		for (bool b : vec_threadsDone)
		{
			if (b == false)
			{
				done_loading = false;
				break;
			}
		}
	} while (!done_loading);
	
	for (auto texture : vec_texturesToLoadToGPU)
	{
		if (!pTextureManager->LoadTextureToGPU(texture, true)) // NEED TO GENERATE MIP MAPS
		{
			std::cerr << "Failed to load texture " << texture->getFileNameFullPath() << " to GPU" << std::endl;
		}
	}

	const float SHOOT_COOLDOWN_TIME = 1.0f;
	float shoot_cooldown_timer = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Timing
		{
			tt = (float)glfwGetTime();
			dt = tt - lastTime;
			lastTime = tt;

			if (dt >= MAX_DELTA_TIME)
				dt = MAX_DELTA_TIME;

			if (pKeyboardManager->keyDown('G'))
				dt *= 10;

			shoot_cooldown_timer -= dt;
		}

		// imgui
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			static float dt_scale = 1.0f;
			ImGui::Begin("Debug Information");
			ImGui::Text("Position: %s", std::to_string(camera->position).c_str());
			ImGui::Text("Facing: %s", std::to_string(camera->forward).c_str());
			ImGui::Checkbox("Draw Debug(~)", &cWorld::debugMode);
			ImGui::SliderFloat("dt Scale", &dt_scale, 0.001f, 2.0f);
			if (ImGui::Button("Reset dt Scale"))
				dt_scale = 1.0f;
			dt *= dt_scale;

			ImGui::Text("dt = %0.3fs", dt);
			ImGui::Text("Avg %0.3f ms/frame (%0.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

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

			camera->forward.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->forward.y = sin(glm::radians(camera->pitch));
			camera->forward.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
			camera->forward = glm::normalize(camera->forward);
			camera->right = glm::normalize(glm::cross(camera->forward, camera->up));
		}

		// keyboard inputs
		{
			if (pKeyboardManager->keyPressed(GLFW_KEY_GRAVE_ACCENT))
				cWorld::debugMode = !cWorld::debugMode;

			int xmov = pKeyboardManager->keyDown(GLFW_KEY_W) - pKeyboardManager->keyDown(GLFW_KEY_S);
			int ymov = 0;
			int zmov = pKeyboardManager->keyDown(GLFW_KEY_D) - pKeyboardManager->keyDown(GLFW_KEY_A);

			if (xmov || ymov || zmov)
			{
				// Move player character when pressing directions
				physicscharacter->Move((glm::normalize(glm::cross(camera->up, camera->right)) * (float)xmov + camera->right * (float)zmov) * 0.3f, dt);
			}
			else
			{
				glm::vec3 new_vel;
				physicscharacter->GetVelocity(new_vel);
				new_vel.x = 0;
				// HACK: keep y velocity so gravity works...
				new_vel.z = 0;
				physicscharacter->Move(new_vel, dt);
			}

			if (pKeyboardManager->keyPressed(GLFW_KEY_SPACE) && physicscharacter->CanJump())
			{
				physicscharacter->Jump();
			}

			// shooting
			if (left_mouse_down && shoot_cooldown_timer <= 0.0f)
			{
				shoot_cooldown_timer = SHOOT_COOLDOWN_TIME;
				nPhysics::sBallDef bulletdef;
				bulletdef.Position = camera->position + camera->forward * 5.0f;
				bulletdef.Mass = 1.0f;
				bulletdef.Radius = 0.2f;
				bulletdef.Elasticity = 1.0f;
				bulletdef.InitialVelocity = camera->forward * 100.0f;

				auto physbullet = pPhysicsFactory->CreateBall(bulletdef);
				pPhysicsManager->getWorld()->AddComponent(physbullet);
				
				auto bullet = new cBulletGameObject();
				bullet->graphics.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
				bullet->graphics.visible = true;
				bullet->graphics.lighting = true;
				
				bullet->physics = physbullet;
				bullet->mesh = new cMeshComponent();
				bullet->mesh->scale = 0.2f;
				bullet->mesh->meshName = "sphere";
				bullet->name = "bullet";

				world->addGameObject(bullet);
			}
		}

		// shader uniforms
		{
			glUseProgram(program);
			// FOV, aspect ratio, near clip, far clip
			p = glm::perspective(glm::radians(fov), ratio, 0.1f, 1000.0f);
			v = glm::lookAt(camera->position, camera->position + camera->forward, camera->up);

			glBindFramebuffer(GL_FRAMEBUFFER, fbo->ID);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//fbo->clearBuffers();

			glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
			glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
			glUniform4f(eyeLocation_loc, camera->position.x, camera->position.y, camera->position.z, 1.0f);
			glUniform4f(pShader->getUniformLocID("ambientColour"), ambience.r, ambience.g, ambience.b, ambience.a);
			glUniform1f(pShader->getUniformLocID("passCount"), 1);
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
			matWorld *= glm::translate(glm::mat4(1.0f), camera->position);
			glUniformMatrix4fv(pShader->getUniformLocID("matModel"), 1, GL_FALSE, glm::value_ptr(matWorld));
			glUniformMatrix4fv(pShader->getUniformLocID("matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(glm::inverse(glm::transpose(matWorld))));
			glUniform4f(pShader->getUniformLocID("diffuseColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), dt, tt, 1.0f, 0.0f);
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

		// update
		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			world->vecGameObjects[i]->update(dt, tt);
		}

		physWorld->Update(dt);

		glm::mat4 t;
		physicscharacter->GetTransform(t);
		character->transform.position.x = glm::mix(character->transform.matWorld[3][0], t[3][0], dt * 5.0f);
		character->transform.position.y = glm::mix(character->transform.matWorld[3][1], t[3][1], dt * 10.0f);
		character->transform.position.z = glm::mix(character->transform.matWorld[3][2], t[3][2], dt * 5.0f);
		character->transform.orientation = glm::cross(camera->right, camera->up);
		character->transform.updateMatricis();
		camera->position = glm::vec3(character->transform.matWorld[3]) + glm::vec3(0.0, 7.0f, 0.0);

		// pre frame, then render
		{
			for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
			{
				world->vecGameObjects[i]->preFrame(dt, tt);
			}
			for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
			{
				world->vecGameObjects[i]->render(dt, tt);
			}
		}

		
		// render fbo to tri
		{
			glUseProgram(program);

			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// whole scene is drawn to buffer
			// 1. disable the FBO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// 2. clear actual screen buffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// 3. use the FBO colour buffer as the texture on the tri
			glActiveTexture(GL_TEXTURE0 + 50);
			glBindTexture(GL_TEXTURE_2D, fbo->colourBuffer_0_ID);
			glUniform1i(pShader->getUniformLocID("secondPassColourSamp"), 50);

			glActiveTexture(GL_TEXTURE0 + 51);
			glBindTexture(GL_TEXTURE_2D, fbo->worldNormalBuffer_1_ID);
			glUniform1i(pShader->getUniformLocID("secondPassWorldNormalSamp"), 51);

			glActiveTexture(GL_TEXTURE0 + 52);
			glBindTexture(GL_TEXTURE_2D, fbo->worldVertexPositionBuffer_2_ID);
			glUniform1i(pShader->getUniformLocID("secondPassWorldVertexPositionSamp"), 52);

			glActiveTexture(GL_TEXTURE0 + 53);
			glBindTexture(GL_TEXTURE_2D, fbo->specularBuffer_3_ID);
			glUniform1i(pShader->getUniformLocID("secondPassSpecularSamp"), 53);

			// Fullscreen overlay
			glActiveTexture(GL_TEXTURE0 + 60);
			glBindTexture(GL_TEXTURE_2D, pTextureManager->getTextureIDFromName("crosshair.bmp"));
			glUniform1i(pShader->getUniformLocID("fullScreenOverlaySamp"), 60);

			glUniform1f(pShader->getUniformLocID("passCount"), 2);

			glUniform4f(pShader->getUniformLocID("diffuseColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("specularColour"), 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(pShader->getUniformLocID("params1"), 0.0f, 0.0f, 0.0f, 0.0f);
			glUniform4f(pShader->getUniformLocID("params2"), 0.0f, 0.0f, 0.0f, 0.0f);

			glm::mat4 matWorld(1.0f);
			matWorld *= glm::translate(glm::mat4(1.0f), camera->position - camera->forward * -2.0f);
			matWorld *= glm::mat4(glm::quatLookAt(camera->forward, glm::normalize(glm::cross(camera->forward, camera->right))));
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
		}

		// draw debug
		if (cWorld::debugMode)
		{
			cWorld::pDebugRenderer->RenderDebugObjects(v, p, dt);
		}

		// need to render each frame even if not rendering
		ImGui::Render();
		if (cWorld::debugMode)
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window); // Draws to screen

		world->doDeferredActions();
		cKeyboardManager::update();
	}

	// Delete everything
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();

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
		delete cWorld::pDebugRenderer;

		delete fbo;

		delete pPhysicsManager;
	}

	return 0;
}
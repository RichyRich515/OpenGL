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
#include "cLight.hpp"
#include "Physics.hpp"
#include "cAABB.hpp"
#include "cKeyboardManager.hpp"

#include "iGameObjectFactory.hpp"
#include "cFactoryManager.hpp"

#include "DebugRenderer/cDebugRenderer.h"

#include "cWorld.hpp"

#include "cParticleEmitter.hpp"

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

float CAMERA_SPEED = 200.0f;

float cameraSensitivity = 0.1f;

GLuint program = 0;

std::map<std::string, cMesh*> mapMeshes;

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
	srand(time(NULL));
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

	std::string textureName = "X-Wing-Texture.bmp";
	if (!pTextureManager->Create2DTextureFromBMPFile(textureName, true)) // NEED TO GENERATE MIP MAPS
	{
		std::cerr << "Failed to load texture " << textureName << " to GPU" << std::endl;
	}

	std::string err;
	if (!pTextureManager->CreateCubeTextureFromBMPFiles(
		"skybox",
		"SpaceBox_right1_posX.bmp",
		"SpaceBox_left2_negX.bmp",
		"SpaceBox_top3_posY.bmp",
		"SpaceBox_bottom4_negY.bmp",
		"SpaceBox_front5_posZ.bmp",
		"SpaceBox_back6_negZ.bmp",
		true, err)) // NEED TO GENERATE MIP MAPS
	{
		std::cerr << "Failed to load cubemap to GPU: " << err << std::endl;
	}

	cGameObject* debugSphere = new cGameObject("debugsphere");
	debugSphere->meshName = "sphere";
	debugSphere->inverseMass = 0.0f;
	debugSphere->wireFrame = true;
	debugSphere->lighting = true;


	openSceneFromFile("scene1.json");


	double cursorX, cursorY;
	float lastcursorX = 0, lastcursorY = 0;
	glfwSetCursorPos(window, 0, 0);

	float ratio;
	int width, height;
	glm::mat4 v, p;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 4000.0f);

	float fov = 60.0f;
	float minfov = 60.0f;

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH);			// Enable depth
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// timing
	float totalTime;
	float lastTime = 0;
	float dt;

	int attack_state = 0;

	cGameObject* destroyer = world->vecGameObjects[0];
	cGameObject* xwing = world->vecGameObjects[1];
	cGameObject* bullet = world->vecGameObjects[2];
	cGameObject* LH = world->vecGameObjects[3];
	cGameObject* RH = world->vecGameObjects[4];

	bullet->visible = false;

	glm::vec3 attack_start = glm::vec3(-400.0f, 200.0f, 0.0f);
	glm::vec3 attack_end = glm::vec3(400.0f, -200.0f, 0.0f);
	glm::vec3 attack_difference = attack_end - attack_start;
	glm::vec3 attack_direction = glm::normalize(attack_difference);

	float attack_length = glm::length(attack_difference);
	float attack_step = 4.0f;
	std::vector<glm::vec3> attack_points;

	float xwing_speed = 800.0f;
	float traveled = 0.0f;
	float bullet_traveled = 0.0f;
	float bullet_to_travel = 0.0f;

	int LH_health = 100;
	int RH_health = 100;

	float dead_offset = 0.0f;
	glUniform1f(glGetUniformLocation(program, "offset"), dead_offset);

	constexpr float MIN_DISTANCE_CHECK_PHYSICS = 1600.0f;

	while (!glfwWindowShouldClose(window))
	{
		totalTime = (float)glfwGetTime();
		dt = totalTime - lastTime;
		lastTime = totalTime;

		if (dt >= MAX_PHYSICS_DELTA_TIME)
			dt = MAX_PHYSICS_DELTA_TIME;

		if (!attack_state)
		{
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

			int xMove = pKeyboardManager->keyDown(GLFW_KEY_A) - pKeyboardManager->keyDown(GLFW_KEY_D);
			int yMove = pKeyboardManager->keyDown(GLFW_KEY_SPACE) - pKeyboardManager->keyDown(GLFW_KEY_C);
			int zMove = pKeyboardManager->keyDown(GLFW_KEY_W) - pKeyboardManager->keyDown(GLFW_KEY_S);

			cameraEye += zMove * CAMERA_SPEED * dt * cameraFront;
			cameraEye += -xMove * CAMERA_SPEED * dt * glm::normalize(glm::cross(cameraFront, cameraUp));
			cameraEye += yMove * CAMERA_SPEED * dt * cameraUp;
		}
		else
		{
			if (attack_state == 1)
			{
				glm::vec3 translation = attack_direction * xwing_speed * dt;
				traveled += glm::length(translation);
				xwing->position += translation;
				cameraEye = xwing->position + xwing->qOrientation * glm::vec3(0.0f, 4.0f, 80.0f);
				cameraFront = attack_direction;

				if (glm::distance(glm::vec3(0.0f), xwing->position) <= MIN_DISTANCE_CHECK_PHYSICS)
				{
					cMesh* mesh = destroyer->collisionObjectInfo.meshes->first;
					sClosestTriInfo info = findClosestTriToPoint(mesh, xwing->position);

					if (debug_mode)
					{
						pDebugRenderer->addTriangle(
							glm::vec3(
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_1].x,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_1].y,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_1].z),
							glm::vec3(
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_2].x,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_2].y,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_2].z),
							glm::vec3(
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_3].x,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_3].y,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_3].z),
							glm::vec3(1.0f, 0.0f, 0.0f), 0);

						pDebugRenderer->addLine(xwing->position, info.point, glm::vec3(0.0f, 1.0f, 0.0f), 0);
					}
					if (info.distance <= xwing->collisionObjectInfo.radius)
					{
						attack_state = 2;
						xwing->qOrientation = glm::quatLookAt(-attack_direction, glm::vec3(0.0f, 1.0f, 0.0f));
						bullet->position = xwing->position;
						bullet->visible = true;
						traveled = attack_length - traveled; // backtrack

						bullet_traveled = 0.0f;
						bullet_to_travel = attack_length - traveled;
					}

				}
			}
			else if (attack_state == 2)
			{
				glm::vec3 translation = -attack_direction * xwing_speed * dt;
				traveled += glm::length(translation);
				xwing->position += translation;
				cameraEye = xwing->position + xwing->qOrientation * glm::vec3(0.0f, 4.0f, -80.0f);
				cameraFront = attack_direction;
			}

			if (traveled > attack_length)
			{
				attack_state = 0;
			}
		}

		if (bullet->visible)
		{
			glm::vec3 translation = attack_direction * xwing_speed * dt;
			bullet_traveled += glm::length(translation);
			bullet->position += translation;

			if (bullet_traveled > bullet_to_travel)
			{
				// missed
				bullet->visible = false;
			}
			else if (glm::distance(glm::vec3(0.0f), bullet->position) < MIN_DISTANCE_CHECK_PHYSICS)
			{
				bool hit = false;
				cMesh* mesh = destroyer->collisionObjectInfo.meshes->first;

				if (LH_health)
				{
					float d = glm::distance(bullet->position, LH->position);
					if (d < (bullet->collisionObjectInfo.radius + LH->collisionObjectInfo.radius))
					{
						glm::vec3 collisionVector = bullet->position - LH->position;
						glm::vec3 normCollisionVector = glm::normalize(collisionVector);
						float overlap = d - (bullet ->collisionObjectInfo.radius + LH->collisionObjectInfo.radius);
						float halflap = -overlap / 2.0f;
						LH_health -= 25;
						hit = true;
						std::cout << "Bullet hit LH" << std::endl;
						bullet->visible = false;

						cGameObject* ip = new cGameObject("impact_point");
						ip->position = LH->position + normCollisionVector * LH->collisionObjectInfo.radius;
						ip->color = glm::vec4(0.9f, 0.1f, 0.1f, 1.0f);
						ip->meshName = "sphere";
						ip->scale = 2.0f;
						ip->inverseMass = 0.0f;
						ip->collisionShapeType = eCollisionShapeType::NONE;
						world->deferredAddGameObject(ip);
					}
				}

				if (RH_health && !hit)
				{
					float d2 = glm::distance(bullet->position, RH->position);
					if (!hit && d2 < (bullet->collisionObjectInfo.radius + RH->collisionObjectInfo.radius))
					{
						glm::vec3 collisionVector = bullet->position - RH->position;
						glm::vec3 normCollisionVector = glm::normalize(collisionVector);
						float overlap = d2 - (bullet->collisionObjectInfo.radius + RH->collisionObjectInfo.radius);
						float halflap = -overlap / 2.0f;
						RH_health -= 25;
						hit = true;
						std::cout << "Bullet hit RH" << std::endl;
						bullet->visible = false;

						cGameObject* ip = new cGameObject("impact_point");
						ip->position = RH->position + normCollisionVector * RH->collisionObjectInfo.radius;
						ip->color = glm::vec4(0.9f, 0.1f, 0.1f, 1.0f);
						ip->meshName = "sphere";
						ip->scale = 2.0f;
						ip->inverseMass = 0.0f;
						ip->collisionShapeType = eCollisionShapeType::NONE;
						world->deferredAddGameObject(ip);
					}
				}
				
				if (hit && !LH_health && !RH_health)
				{
					std::cout << "You've destroyed the Star Destroyer!" << std::endl;
					xwing->visible = false;
					attack_state = 0;
				}

				if (!hit)
				{
					sClosestTriInfo info = findClosestTriToPoint(mesh, bullet->position);

					if (debug_mode)
					{
						pDebugRenderer->addTriangle(
							glm::vec3(
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_1].x,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_1].y,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_1].z),
							glm::vec3(
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_2].x,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_2].y,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_2].z),
							glm::vec3(
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_3].x,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_3].y,
								mesh->vecVertices[mesh->vecTriangles[info.triIndex].vert_index_3].z),
							glm::vec3(1.0f, 0.0f, 0.0f), 0);

						pDebugRenderer->addLine(bullet->position, info.point, glm::vec3(0.0f, 1.0f, 0.0f), 0);
					}
					if (info.distance <= bullet->collisionObjectInfo.radius)
					{
						// HIT
						std::cout << "Bullet hit" << std::endl;
						bullet->visible = false;

						cGameObject* ip = new cGameObject("impact_point");
						ip->position = info.point;
						ip->color = glm::vec4(0.9f, 0.1f, 0.1f, 1.0f);
						ip->meshName = "sphere";
						ip->scale = 2.0f;
						ip->inverseMass = 0.0f;
						ip->collisionShapeType = eCollisionShapeType::NONE;
						world->deferredAddGameObject(ip);
					}
				}
			}
		}

		glUseProgram(program);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!LH_health && !RH_health)
		{
			dead_offset += 7.5f * dt;
			glUniform1f(glGetUniformLocation(program, "offset"), dead_offset);
		}


		if (pKeyboardManager->keyPressed(GLFW_KEY_GRAVE_ACCENT))
		{
			debug_mode = !debug_mode;
		}
		if (pKeyboardManager->keyPressed(GLFW_KEY_2))
		{
			attack_state = 1;

			int dir = rand() % 2;

			while (true)
			{
				attack_start = glm::normalize(glm::vec3(dir ? randInRange(-1.0f, 0.0f) : randInRange(0.0f, 1.0f), randInRange(-0.2f, 0.2f), randInRange(-0.6f, 0.6f))) * randInRange(0.0f, 3000.0f);
				if (CheckIfPointInMeshAndRadius(destroyer->collisionObjectInfo.meshes->first, attack_start, xwing->collisionObjectInfo.radius))
					std::cout << "attack_start inside star destroyer, repicking" << std::endl;
				else
					break;
			}
			while (true)
			{
				attack_end = glm::normalize(glm::vec3(dir ? randInRange(0.0f, 1.0f) : randInRange(-1.0f, 0.0f), randInRange(-0.2f, 0.2f), randInRange(-0.6f, 0.6f))) * randInRange(0.0f, 3000.0f);
				if (CheckIfPointInMeshAndRadius(destroyer->collisionObjectInfo.meshes->first, attack_end, xwing->collisionObjectInfo.radius))
					std::cout << "attack_end inside star destroyer, repicking" << std::endl;
				else
					break;
			}

			attack_difference = attack_end - attack_start;
			attack_direction = glm::normalize(attack_difference);
			attack_length = glm::length(attack_difference);

			attack_points.clear();
			for (float s = 0.0f; s < attack_length; s += attack_step)
				attack_points.push_back(attack_start + attack_direction * s);

			traveled = 0.0f;
			xwing->position = attack_start;
			xwing->qOrientation = glm::quatLookAt(attack_direction, glm::vec3(0.0f, 1.0f, 0.0f));
			bullet->qOrientation = xwing->qOrientation;
			bullet->visible = false;
		}
		else if (pKeyboardManager->keyPressed(GLFW_KEY_3))
		{
			attack_state = 1;

			int dir = rand() % 2;
			attack_start = glm::vec3(500.0f * (dir ? -1 : 1), 240.0f, 560.0f);
			attack_end = glm::vec3(500.0f * (dir ? 1 : -1), 240.0f, 560.0f);

			attack_difference = attack_end - attack_start;
			attack_direction = glm::normalize(attack_difference);
			attack_length = glm::length(attack_difference);

			attack_points.clear();
			for (float s = 0.0f; s < attack_length; s += attack_step)
				attack_points.push_back(attack_start + attack_direction * s);

			traveled = 0.0f;
			xwing->position = attack_start;
			xwing->qOrientation = glm::quatLookAt(attack_direction, glm::vec3(0.0f, 1.0f, 0.0f));
			bullet->qOrientation = xwing->qOrientation;
			bullet->visible = false;
		}

		if (world->vecGameObjects.size() && world->vecLights.size())
		{
			if (LH_health || RH_health)
			{
				std::ostringstream windowTitle;
				windowTitle << std::boolalpha << std::fixed << std::setprecision(2)
					<< "attack_state: " << attack_state << ", Health: LH " << LH_health << "% RH " << RH_health << "%";
				glfwSetWindowTitle(window, windowTitle.str().c_str());
			}
			else
			{
				glfwSetWindowTitle(window, "You've destroyed the Star Destroyer!");
			}
		}

		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			world->vecGameObjects[i]->update(dt);
			world->vecGameObjects[i]->physicsUpdate(dt);
			world->vecGameObjects[i]->updateMatricis();
		}

		v = glm::lookAt(cameraEye, cameraEye + cameraFront, cameraUp);

		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(p));
		glUniform4f(eyeLocation_loc, cameraEye.x, cameraEye.y, cameraEye.z, 1.0f);

		// draw Skybox
		{
			// Tie texture
			GLuint texture_ul = pTextureManager->getTextureIDFromName("skybox");
			if (texture_ul)
			{
				glActiveTexture(GL_TEXTURE10);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ul);
				glUniform1i(glGetUniformLocation(program, "skyboxSamp00"), 10);
			}
			debugSphere->scale = 1.0f;
			debugSphere->position = cameraEye;
			debugSphere->wireFrame = false;
			debugSphere->color = glm::vec4(1.0f);
			debugSphere->updateMatricis();
			glUniformMatrix4fv(glGetUniformLocation(program, "matModel"), 1, GL_FALSE, glm::value_ptr(debugSphere->matWorld));
			glUniformMatrix4fv(glGetUniformLocation(program, "matModelInverseTranspose"), 1, GL_FALSE, glm::value_ptr(debugSphere->inverseTransposeMatWorld));
			glUniform4f(glGetUniformLocation(program, "diffuseColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(glGetUniformLocation(program, "specularColour"), 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(glGetUniformLocation(program, "params1"), dt, totalTime, 1.0f, 0.0f);
			glUniform4f(glGetUniformLocation(program, "params2"), 1.0f, 0.0f, 0.0f, 0.0f);

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

		if (attack_state)
		{
			debugSphere->lighting = false;
			debugSphere->scale = 2.0f;

			debugSphere->position = attack_start;
			debugSphere->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);
			debugSphere->updateMatricis();
			drawObject(debugSphere, program, pVAOManager, dt, totalTime);

			debugSphere->position = attack_end;
			debugSphere->color = glm::vec4(1.0f, 1.0f, 0.2f, 1.0f);
			debugSphere->updateMatricis();
			drawObject(debugSphere, program, pVAOManager, dt, totalTime);

			debugSphere->scale = 0.33f;
			debugSphere->color = glm::vec4(1.0f, 0.65f, 0.2f, 1.0f);
			for (unsigned i = 0; i < attack_points.size(); ++i)
			{
				debugSphere->position = attack_points[i];
				debugSphere->updateMatricis();
				drawObject(debugSphere, program, pVAOManager, dt, totalTime);
			}
		}

		for (unsigned i = 0; i != world->vecGameObjects.size(); ++i)
		{
			if (!world->vecGameObjects[i]->visible)
				continue;
			if (i > 0 && !LH_health && !RH_health)
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
	GLuint texture_ul = pTextureManager->getTextureIDFromName(go->textureName);
	if (texture_ul)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_ul);
		glUniform1i(glGetUniformLocation(shader, "textSamp00"), 0);
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
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

static const char* vertex_shader_text = "\
uniform mat4 MVP;\n\
attribute vec3 vCol;\n\
attribute vec3 vPos;\n\
varying vec3 color;\n\
void main()\n\
{\n\
    gl_Position = MVP * vec4(vPos, 1.0);\n\
	color = vCol;\n\
}\n";

static const char* fragment_shader_text =
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

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
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;
	
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

	if (!pModelLoader->loadModel("assets/models/bun_zipper_res4_xyz.ply", mesh))
	{
		std::cerr << "Failed to load Model" << std::endl;
		exit(EXIT_FAILURE);
	}

	/*
	unsigned numVerts = mesh->vecTriangles.size() * 3;
	vertices = new sVertex[numVerts];

	for (unsigned i = 0, vertIndex = 0; i < mesh->vecTriangles.size(); ++i, vertIndex += 3)
	{
		sPlyVertex t = mesh->vecVertices[mesh->vecTriangles[i].vert_index_1];
		vertices[vertIndex] = sVertex{ t.x, t.y, t.z, 1.0f, 1.0f, 1.0f };
		t = mesh->vecVertices[mesh->vecTriangles[i].vert_index_2];
		vertices[vertIndex + 1] = sVertex{ t.x, t.y, t.z, 1.0f, 1.0f, 1.0f };
		t = mesh->vecVertices[mesh->vecTriangles[i].vert_index_3];
		vertices[vertIndex + 2] = sVertex{ t.x, t.y, t.z, 1.0f, 1.0f, 1.0f };
	}

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(sVertex), vertices, GL_STATIC_DRAW);
	*/

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	program = glCreateProgram();

	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glUseProgram(program);

	mvp_location = glGetUniformLocation(program, "MVP");

	// Default
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Also see GL_POINT

	sModelDrawInfo drawInfo;
	if (!pVAOManager->LoadModelIntoVAO("Bunny", mesh, drawInfo, program))
	{
		std::cerr << "Failed to load model to GPU" << std::endl;
		exit(EXIT_FAILURE);
	}

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

		m = glm::mat4(1.0f);

		glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 4.0f, 4.0f));
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));

		m *= rotationX * rotationY * rotationZ * translation * scale; 

		// FOV, aspect ratio, near clip, far clip
		p = glm::perspective(glm::radians(60.0f), ratio, 0.1f, 1000.0f);

		v = glm::mat4(1.0f);
		v = glm::lookAt(cameraEye, cameraEye + cameraFront, cameraUp);

		mvp = p * v * m; // Dont change this order.

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
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

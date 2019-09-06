// Main.cpp
// Entrypoint for program
// 2019-09-04

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

static const struct Vertex
{
	float x, y;
	float r, g, b;
};
std::vector<Vertex> vertices;

static const char* vertex_shader_text =
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	float x1 = 60.0f;
	float x2 = -60.0f;
	float x3 = 0;
	float y1 = -30.0f;
	float y2 = 60.f;

	vertices.push_back(Vertex{ x1, y1, 1.0f, 0.0f, 0.0f });
	vertices.push_back(Vertex{ x2, y1, 0.0f, 1.0f, 0.0f });
	vertices.push_back(Vertex{ x3, y2, 0.0f, 0.0f, 1.0f });
	for (int i = 0; i < 11; i++)
	{
		x1 = x1 / -2;
		x2 = x2 / -2;
		x3 = x3 / -2;
		y1 = y1 / -2;
		y2 = y2 / -2;
		vertices.push_back(Vertex{ x1, y1, 1.0f, 0.0f, 0.0f });
		vertices.push_back(Vertex{ x2, y1, 0.0f, 1.0f, 0.0f });
		vertices.push_back(Vertex{ x3, y2, 0.0f, 0.0f, 1.0f });
	}
	
	// NOTE: OpenGL error checks have been omitted for brevity
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

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

	mvp_location = glGetUniformLocation(program, "MVP");
	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void*)(sizeof(float) * 2));


	glm::vec3 cameraEye = glm::vec3(0.0, 15.0, -150.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 15.0f, 0.0f);

	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	float startTime = glfwGetTime();
	float lastTime = glfwGetTime();
	float dt;


	float cameraMovement = -10;
	while (!glfwWindowShouldClose(window))
	{
		dt = lastTime - glfwGetTime();
		lastTime = glfwGetTime();
		float ratio;
		int width, height;
		glm::mat4 m, p, v, mvp;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		m = glm::mat4(1.0f);

		glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		m *= translate * scale * rotateZ;

		// FOV, aspect ratio, near clip, far clip
		p = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);

		v = glm::mat4(1.0f);

		// Move camera
		if (cameraEye.z < 0)
		{
			cameraEye.y += dt * -cameraMovement / 10;
			cameraTarget.y += dt * -cameraMovement / 10;
		}
		else
		{
			cameraEye.y -= dt * -cameraMovement / 10;
			cameraTarget.y -= dt * -cameraMovement / 10;
		}
		if (cameraEye.z > 150)
			cameraMovement = 10;
		else if (cameraEye.z < -150)
			cameraMovement = -10;
		cameraEye.z += dt * cameraMovement;
		std::cout << cameraEye.z << std::endl;
		v = glm::lookAt(cameraEye, cameraTarget, upVector);

		mvp = m * v * p;

		glUseProgram(program);

		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

		// Wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Default
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// Also see GL_POINT

		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

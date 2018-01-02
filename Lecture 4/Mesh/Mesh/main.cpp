#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "common/shader.hpp"
#include <vector>
#include "common\objloader.hpp"

using namespace glm;

int vertexCount = 0;

GLFWwindow* InitWindow()
{
	if (!glfwInit())
	{
		fprintf(stderr, "GLFW failed!\n");
		return NULL;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = NULL;
	window = glfwCreateWindow(1024, 768, "Lecture 2 - Triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return NULL;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	return window;
}

GLuint SetVAO()
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	loadOBJ("bunny.obj", vertices, uvs, normals);

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint vcb;
	glGenBuffers(1, &vcb);
	glBindBuffer(GL_ARRAY_BUFFER, vcb);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Set vertex memory structure.
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vcb);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	vertexCount = vertices.size();
	return vao;
}

int main(void)
{
	GLFWwindow* window = InitWindow();
	if (window == NULL)
	{
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint vao = SetVAO();

	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	GLuint MMatrixID = glGetUniformLocation(programID, "MM");
	GLuint VMatrixID = glGetUniformLocation(programID, "VM");
	GLuint PMatrixID = glGetUniformLocation(programID, "PV");

	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		4.0f / 3.0f,
		0.1f,
		100.0f);

	glm::mat4 view = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	glm::mat4 model = glm::mat4(1.0f);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(VMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(PMatrixID, 1, GL_FALSE, &projection[0][0]);

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		glfwSwapBuffers(window);

		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
}
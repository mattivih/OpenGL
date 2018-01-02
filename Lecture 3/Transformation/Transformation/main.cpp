#define GLEW_STATIC

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "common/shader.hpp"

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

int main(void)
{
	GLFWwindow* window = InitWindow();
	if (window == NULL)
	{
		return -1;
	}

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(0);

	// Shader commonista ehka
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	GLuint MMatrixID = glGetUniformLocation(programID, "MM");
	GLuint VMatrixID = glGetUniformLocation(programID, "VM");
	GLuint PMatrixID = glGetUniformLocation(programID, "PV");

	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		4.0f / 3.0f,
		0.1f,
		100.0f
	);

	glm::mat4 view = glm::lookAt(
		glm::vec3(4, 3, 3),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	glm::mat4 tra = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);

	int t = 0;

	do
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		float z = (float)glm::sin((double)t * 0.01) * 0.01f;
		tra = glm::translate(tra, glm::vec3(0.0f, 0.0f, z));
		rot = glm::rotate(rot, 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = tra * rot;

		glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(VMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(PMatrixID, 1, GL_FALSE, &projection[0][0]);

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		glfwPollEvents();

		++t;
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
}
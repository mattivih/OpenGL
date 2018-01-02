#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "common/shader.hpp"
#include "common\objloader.hpp"
#include "common/texture.hpp"

using namespace glm;

int vertexCount = 0;

glm::vec3 lightDirection;
glm::vec3 lightColor;
glm::vec3 specularColor;
glm::vec3 ambientColor;
glm::mat3 invTranspose;
glm::vec3 worldSpaceCameraPosition;
glm::vec3 worldSpaceCameraTarget;

float shininess = 10.0f;
float specularAmount = 20.0f;
float lightAmount = 1.0f;
float ambientAmount = 1.0f;


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
	window = glfwCreateWindow(1024, 768, "Lecture 5 - Texture Mapping", NULL, NULL);
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint vno;
	glGenBuffers(1, &vno);
	glBindBuffer(GL_ARRAY_BUFFER, vno);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

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
	glBindBuffer(GL_ARRAY_BUFFER, vno);
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
	GLuint ITMatrixID = glGetUniformLocation(programID, "IT");
	GLuint AmbientAmountID = glGetUniformLocation(programID, "AmbientAmount");
	GLuint AmbientColorID = glGetUniformLocation(programID, "AmbientColor");
	GLuint LightAmountID = glGetUniformLocation(programID, "LightAmount");
	GLuint LightColorID = glGetUniformLocation(programID, "LightColor");
	GLuint LightDirectionID = glGetUniformLocation(programID, "LightDirection");

	GLuint SpecularColorID = glGetUniformLocation(programID, "SpecularColor");
	GLuint WorldSpaceCameraDirectionID = glGetUniformLocation(programID, "WorldSpaceCameraDirection");
	GLuint ShininessID = glGetUniformLocation(programID, "Shininess");
	GLuint SpecularAmountID = glGetUniformLocation(programID, "SpecularAmount");

	glm::mat4 projection = glm::perspective(
		glm::radians(45.0f),
		4.0f / 3.0f,
		0.1f,
		100.0f);

	worldSpaceCameraPosition.x = 0.0f;
	worldSpaceCameraPosition.y = 0.0f;
	worldSpaceCameraPosition.z = 0.0f;
	worldSpaceCameraTarget.x = 4.0f;
	worldSpaceCameraTarget.y = 3.0f;
	worldSpaceCameraTarget.z = 3.0f;

	glm::mat4 view = glm::lookAt(
		worldSpaceCameraTarget,
		worldSpaceCameraPosition,
		glm::vec3(0, 1, 0)
	);

	glm::mat4 model = glm::mat4(1.0f);

	ambientColor.r = 1.0f;
	ambientColor.g = 1.0f;
	ambientColor.b = 1.0f;

	lightDirection.x = 0.5f;
	lightDirection.y = 0.5f;
	lightDirection.z = 0.5f;

	lightColor.x = 1.0f;
	lightColor.y = 1.0f;
	lightColor.z = 1.0f;

	specularColor.x = 1.0f;
	specularColor.y = 1.0f;
	specularColor.z = 1.0f;

	glm::vec3 cameraDirection = glm::normalize(worldSpaceCameraPosition - worldSpaceCameraTarget);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(VMatrixID, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(PMatrixID, 1, GL_FALSE, &projection[0][0]);

		invTranspose = glm::transpose(glm::inverse(glm::mat3(model)));
		glUniformMatrix3fv(ITMatrixID, 1, GL_FALSE, &invTranspose[0][0]);

		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);

		if (glfwGetKey(window, GLFW_KEY_Q))
		{
			ambientAmount += 0.01f;
		}

		if (glfwGetKey(window, GLFW_KEY_W))
		{
			ambientAmount -= 0.01f;
		}

		if (glfwGetKey(window, GLFW_KEY_E))
		{
			shininess += 0.01f;
		}

		if (glfwGetKey(window, GLFW_KEY_R))
		{
			shininess -= 0.01f;
		}

		glUniform1f(AmbientAmountID, ambientAmount);
		glUniform3f(
			AmbientColorID,
			ambientColor.x,
			ambientColor.y,
			ambientColor.z);
		glUniform1f(LightAmountID, lightAmount);
		glUniform3f(
			LightColorID,
			lightColor.x,
			lightColor.y,
			lightColor.z);
		glUniform3f(
			LightDirectionID,
			lightDirection.x,
			lightDirection.y,
			lightDirection.z);
		glUniform3f(
			SpecularColorID,
			specularColor.x,
			specularColor.y,
			specularColor.z);
		glUniform3f(
			WorldSpaceCameraDirectionID,
			cameraDirection.x,
			cameraDirection.y,
			cameraDirection.z);
		glUniform1f(
			ShininessID,
			shininess);
		glUniform1f(
			SpecularAmountID,
			specularAmount);

		glfwSwapBuffers(window);

		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glfwTerminate();
}
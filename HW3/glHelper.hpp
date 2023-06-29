#include <fstream>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "dataTypes.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

int gWidth, gHeight;
glm::mat4 projectionMatrix;

bool ReadDataFromFile(const string& fileName, string& data) {
	fstream myfile;

	myfile.open(fileName.c_str(), ios::in);

	if (myfile.is_open())
	{
		string curLine;

		while (getline(myfile, curLine))
		{
			data += curLine;
			if (!myfile.eof())
			{
				data += "\n";
			}
		}

		myfile.close();
	}
	else
	{
		return false;
	}

	return true;
}

GLuint createVS(const char* shaderName) {
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFS(const char* shaderName) {
	string shaderSource;

	string filename(shaderName);
	if (!ReadDataFromFile(filename, shaderSource))
	{
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);

	return fs;
}

GLuint initShader(string vertShader, string fragShader) {
	GLuint gProgram = glCreateProgram();
	GLuint vs = createVS(vertShader.c_str());
	GLuint fs = createFS(fragShader.c_str());

	glAttachShader(gProgram, vs);
	glAttachShader(gProgram, fs);

	glLinkProgram(gProgram);

	GLint status;
	glGetProgramiv(gProgram, GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}
	return gProgram;
}

GLuint createCubeMap(string right, string left, string top, string bottom, string front, string back)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int rWidth, lWidth, tWidth, bWidth, fWidth, baWidth;
	int rHeight, lHeight, tHeight, bHeight, fHeight, baHeight;
	int rChannels, lChannels, tChannels, bChannels, fChannels, baChannels;

	unsigned char* rData = stbi_load(right.c_str(), &rWidth, &rHeight, &rChannels, 0);
	unsigned char* lData = stbi_load(left.c_str(), &lWidth, &lHeight, &lChannels, 0);
	unsigned char* tData = stbi_load(top.c_str(), &tWidth, &tHeight, &tChannels, 0);
	unsigned char* bData = stbi_load(bottom.c_str(), &bWidth, &bHeight, &bChannels, 0);
	unsigned char* fData = stbi_load(front.c_str(), &fWidth, &fHeight, &fChannels, 0);
	unsigned char* baData = stbi_load(back.c_str(), &baWidth, &baHeight, &baChannels, 0);

	if (rData)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, rWidth, rHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, rData);
	else
		cout << "Could not load right texture" << endl;
	if (lData)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, lWidth, lHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, lData);
	else
		cout << "Could not load left texture" << endl;
	if (tData)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tData);
	else
		cout << "Could not load top texture" << endl;
	if (bData)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, bWidth, bHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bData);
	else
		cout << "Could not load bottom texture" << endl;
	if (fData)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, fWidth, fHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, fData);
	else
		cout << "Could not load front texture" << endl;
	if (baData)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, baWidth, baHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, baData);
	else
		cout << "Could not load right texture" << endl;

	stbi_image_free(rData);
	stbi_image_free(lData);
	stbi_image_free(tData);
	stbi_image_free(bData);
	stbi_image_free(fData);
	stbi_image_free(baData);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	return textureID;
}

void clearDisplay() {

	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

GLFWwindow* InitilizeWindow(int width, int height) {
	GLFWwindow* window;
	if (!glfwInit())
	{
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (GLEW_OK != glewInit())
	{
		cout << "Failed to initialize GLEW" << endl;
		exit(-1);
	}

	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER));
	strcat(rendererInfo, " - ");
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION));
	glfwSetWindowTitle(window, rendererInfo);

	return window;
}

void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);

	float fovyRad = (float)(45.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, 1.0f, 1.0f, 100.0f);
}


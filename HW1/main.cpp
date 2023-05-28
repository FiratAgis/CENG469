#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

struct Surface{
	Surface(GLint inX, GLint inY,
		GLfloat inX00, GLfloat inX01, GLfloat inX02, GLfloat inX03,
		GLfloat inX10, GLfloat inX11, GLfloat inX12, GLfloat inX13,
		GLfloat inX20, GLfloat inX21, GLfloat inX22, GLfloat inX23,
		GLfloat inX30, GLfloat inX31, GLfloat inX32, GLfloat inX33){
			surf[0][0] = inX00;
			surf[0][1] = inX01;
			surf[0][2] = inX02;
			surf[0][3] = inX03;

			surf[1][0] = inX10;
			surf[1][1] = inX11;
			surf[1][2] = inX12;
			surf[1][3] = inX13;

			surf[2][0] = inX20;
			surf[2][1] = inX21;
			surf[2][2] = inX22;
			surf[2][3] = inX23;

			surf[3][0] = inX30;
			surf[3][1] = inX31;
			surf[3][2] = inX32;
			surf[3][3] = inX33;
			x = inX;
			y = inY;
		}
	glm::mat4 surf;
	GLint x, y;
};

vector<Surface> gSurfaces;

GLuint gVertexAttribBuffer, gIndexBuffer;

GLuint gProgram;
int gWidth, gHeight;

GLint lightPosLoc;
GLint ILoc;
GLint lightNoLoc;

GLint modelingMatrixLoc;
GLint viewingMatrixLoc;
GLint projectionMatrixLoc;
GLint eyePosLoc;

GLint scaleLoc;
GLint curveAmountLoc;

GLint surfDataLoc[6][6];

glm::vec3 lightPos[5];
glm::vec3 Ival[5];
GLint lightNo;

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;
glm::vec3 eyePos(0, 0, 2);

glm::ivec2 curveAmount;
GLfloat scale = 1.0;

int samplingRate = 10;
int x_limit, y_limit;
map<int, GLuint> vao_map;
GLint currentVAO;
int gVertexTDataSizeInBytes, gVertexCurveIndexDataSizeInBytes, gIndexDataSizeInBytes;

float xang = -30.0;
float yang = 0;
float zang = 0;

float xtrans = 0;
float ytrans = 0;
float ztrans = 0;

bool mode = true;

bool modelUpdated = true;

void rotate(int axis, float amount){
	switch(axis){
		case 0:
			xang += amount;
			break;
		case 1:
			yang += amount;
			break;
		case 2:
			zang += amount;
			break;
	}
	modelUpdated = true;
}

void move(int axis, float amount){
	switch(axis){
		case 0:
			xtrans += amount;
			break;
		case 1:
			ytrans += amount;
			break;
		case 2:
			ztrans += amount;
			break;
	}
	modelUpdated = true;
}

void addLight(string line, int i){
	istringstream this_line(line);
	istream_iterator<float> begin(this_line), end;
    vector<float> array(begin, end);
	lightPos[i] = glm::vec3(array[0], array[1], array[2]);
	Ival[i] = glm::vec3(array[3], array[4], array[5]);
}

bool ParseInput(const string& fileName){
	fstream inFile;

	inFile.open(fileName.c_str(), ios::in);


	if(inFile.is_open()){
		int x, y;
		int i = 0;
		vector<int> intLine;
		vector<float> floatLine;
		GLfloat f1, f2, f3, f4, f5, f6;
		string curLine;

		getline(inFile, curLine);

		stringstream str(curLine);

		str >> lightNo;

		while(i < min(lightNo, 5)){
			getline(inFile, curLine);
			addLight(curLine, i);
			i++;
		}
		i=0;
		while(i < min(lightNo, 5)){
			cout << lightPos[i].x << " " << lightPos[i].y << " " << lightPos[i].z << " " << Ival[i].x << " " << Ival[i].y << " " << Ival[i].z << endl; 
			i++;
		}
		getline(inFile, curLine);
		cout << curLine << endl;
		str.str(curLine);
		str >> x >> y;
		float table[x][y];
		curveAmount = glm::ivec2(x/4, y/4);
		for(int x_counter = 0; x_counter < x; x_counter++){
			getline(inFile, curLine);
			str.str(curLine);
			for(int y_counter = 0; y_counter < y; y_counter++){
				str >> table[x_counter][y_counter];
			}
		}

		for(int x_counter = 0; x_counter < x / 4; x_counter++){
			for (int y_counter = 0; y_counter < y / 4; y_counter++){
				int xval = x_counter * 4;
				int yval = y_counter * 4;
				gSurfaces.push_back(Surface(y_counter, x_counter,
					table[xval + 0][yval + 0], table[xval + 0][yval + 1], table[xval + 0][yval + 2], table[xval + 0][yval + 3],
					table[xval + 1][yval + 0], table[xval + 1][yval + 1], table[xval + 1][yval + 2], table[xval + 1][yval + 3],
					table[xval + 2][yval + 0], table[xval + 2][yval + 1], table[xval + 2][yval + 2], table[xval + 2][yval + 3],
					table[xval + 3][yval + 0], table[xval + 3][yval + 1], table[xval + 3][yval + 2], table[xval + 3][yval + 3]));
			}
		}
		return true;
	}
	else{
		cout << "Could find input file" << endl;
		return false;
	}
}



bool ReadDataFromFile(const string& fileName, string& data){
	fstream myfile;

	myfile.open(fileName.c_str(), std::ios::in);

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

GLuint createVS(const char* shaderName){
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

GLuint createFS(const char* shaderName){
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

void initShaders() {
	cout << "initShaders Called" << endl;
	gProgram = glCreateProgram();
	GLuint vs = createVS("vert_hw1.glsl");
	GLuint fs = createFS("frag_hw1.glsl");

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
}

void initUniform() {
	cout << "initUniform Called" << endl;
	lightPosLoc = glGetUniformLocation(gProgram, "lightLoc");
	ILoc = glGetUniformLocation(gProgram, "I");
	lightNoLoc = glGetUniformLocation(gProgram, "lightNo");

	modelingMatrixLoc = glGetUniformLocation(gProgram, "modelingMatrix");
	viewingMatrixLoc = glGetUniformLocation(gProgram, "viewingMatrix");
	projectionMatrixLoc = glGetUniformLocation(gProgram, "projectionMatrix");
	eyePosLoc = glGetUniformLocation(gProgram, "eyePos");

	scaleLoc = glGetUniformLocation(gProgram, "scale");
	curveAmountLoc = glGetUniformLocation(gProgram, "curveAmount");

	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 6; y++) {
			surfDataLoc[x][y] = glGetUniformLocation(gProgram, (string("Z_z") + to_string(x) + to_string(y)).c_str());
		}
	}

}

void initVBO()
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	vao_map[samplingRate] = vao;
	assert(vao_map[samplingRate] > 0);
	glBindVertexArray(vao_map[samplingRate]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	assert(glGetError() == GL_NONE);

	glGenBuffers(1, &gVertexAttribBuffer);
	glGenBuffers(1, &gIndexBuffer);

	assert(gVertexAttribBuffer > 0 && gIndexBuffer > 0);

	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

	int vertexAmount = gSurfaces.size() * samplingRate * samplingRate;
	int faceAmount = gSurfaces.size() * (samplingRate - 1) * (samplingRate - 1) * 2;

	gVertexTDataSizeInBytes = vertexAmount * 2 * sizeof(GLfloat);
	gVertexCurveIndexDataSizeInBytes = vertexAmount * 2 * sizeof(GLint);

	gIndexDataSizeInBytes = faceAmount * 3 * sizeof(GLuint);
	GLfloat* tData = new GLfloat[vertexAmount * 2];
	GLint* curveIndexData = new GLint[vertexAmount * 2];

	GLuint* indexData = new GLuint[faceAmount * 3];


	int count = 0;
	for (int i = 0; i < gSurfaces.size(); i++)
	{
		for (int j = 0; j < samplingRate; j++) {

			for (int k = 0; k < samplingRate; k++) {
				tData[2 * count] = (1.0 / ((float)samplingRate - 1.0)) * (float)j;
				tData[2 * count + 1] = (1.0 / ((float)samplingRate - 1.0)) * (float)k;
				curveIndexData[2 * count] = gSurfaces[i].x;
				curveIndexData[2 * count + 1] = gSurfaces[i].y;
				count++;
			}
		}
	}

	count = 0;
	for (int i = 0; i < gSurfaces.size(); i++)
	{
		for (int j = 0; j < samplingRate - 1; j++) {

			for (int k = 0; k < samplingRate - 1; k++) {
				indexData[6 * count + 5] = (i * samplingRate * samplingRate) + j * (samplingRate) + k;
				indexData[6 * count + 4] = (i * samplingRate * samplingRate) + j * (samplingRate) + (k + 1);
				indexData[6 * count + 3] = (i * samplingRate * samplingRate) + (j + 1) * (samplingRate) + (k + 1);
				indexData[6 * count + 2] = (i * samplingRate * samplingRate) + (j + 1) * (samplingRate) + (k + 1);
				indexData[6 * count + 1] = (i * samplingRate * samplingRate) + (j + 1) * (samplingRate) + k;
				indexData[6 * count + 0] = (i * samplingRate * samplingRate) + j * (samplingRate) + k;
				count++;
			}
		}
	}


	glBufferData(GL_ARRAY_BUFFER, gVertexTDataSizeInBytes + gVertexCurveIndexDataSizeInBytes, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexCurveIndexDataSizeInBytes, curveIndexData);
	glBufferSubData(GL_ARRAY_BUFFER, gVertexCurveIndexDataSizeInBytes, gVertexTDataSizeInBytes, tData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gIndexDataSizeInBytes, indexData, GL_STATIC_DRAW);

	delete[] curveIndexData;
	delete[] tData;
	delete[] indexData;

	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexCurveIndexDataSizeInBytes));
}

void reshape(GLFWwindow* window, int w, int h)
{
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);

	float fovyRad = (float)(45.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, w / (float)h, 1.0f, 100.0f);


	viewingMatrix = glm::lookAt(glm::vec3(0,0,2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void setInitialUniforms() {
	glUseProgram(gProgram);
	glUniform3fv(lightPosLoc, 5, glm::value_ptr(lightPos[0]));
	glUniform3fv(ILoc, 5, glm::value_ptr(Ival[0]));
	glUniform1i(lightNoLoc, lightNo);

	for (int i = 0; i < gSurfaces.size(); i++) {
		glUniformMatrix4fv(surfDataLoc[gSurfaces[i].x][gSurfaces[i].y], 1, GL_FALSE, glm::value_ptr(gSurfaces[i].surf));
	}

	
	glUniform2iv(curveAmountLoc, 1, glm::value_ptr(curveAmount));
}

void clearDisplay() {

	glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void display()
{
	glUniform1f(scaleLoc, scale);
	glUseProgram(gProgram);
	if (modelUpdated) {
		glm::mat4 matT = glm::translate(glm::mat4(1.0), glm::vec3(xtrans, ytrans, ztrans));
		glm::mat4 matRx = glm::rotate<float>(glm::mat4(1.0), (xang / 180.0) * M_PI, glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 matRy = glm::rotate<float>(glm::mat4(1.0), (yang / 180.0) * M_PI, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 matRz = glm::rotate<float>(glm::mat4(1.0), (zang / 180.0) * M_PI, glm::vec3(0.0, 0.0, 1.0));
		modelingMatrix = matT * matRz * matRy * matRx;

		glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
		glUniformMatrix4fv(viewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
		glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelingMatrix));
		glUniform3fv(eyePosLoc, 1, glm::value_ptr(eyePos));
	}
}

void drawModel()
{
	glBindBuffer(GL_ARRAY_BUFFER, gVertexAttribBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBuffer);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexCurveIndexDataSizeInBytes));

	glDrawElements(GL_TRIANGLES, gSurfaces.size() * (samplingRate - 1) * (samplingRate - 1) * 2 * 3, GL_UNSIGNED_INT, 0);
}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		move(0, 0.1);
	}
	else if(key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		move(0, -0.1);
	}
	else if(key == GLFW_KEY_I && action == GLFW_PRESS)
	{
		move(1, 0.1);
	}
	else if(key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		move(1, -0.1);
	}
	else if(key == GLFW_KEY_U && action == GLFW_PRESS)
	{
		move(2, 0.1);
	}
	else if(key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		move(2, -0.1);
	}
	else if(key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		rotate(0, 5);
	}
	else if(key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		rotate(0, -5);
	}
	else if(key == GLFW_KEY_B && action == GLFW_PRESS)
	{
		rotate(1, 5);
	}
	else if(key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		rotate(1, -5);
	}
	else if(key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		rotate(2, 5);
	}
	else if(key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotate(2, -5);
	}
	else if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
		cout << "xang= " << xang << ", yang= " << yang << ", zang= " << zang << endl;
		cout << "xtrans= " << xtrans << ", ytrans= " << ytrans << ", ztrans= " << ztrans << endl;
		cout << "scale= " << scale << ", samplingRate= " << samplingRate << endl; 
	}
	else if(key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		scale += 0.1;
	}
	else if(key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		scale -= 0.1;
		scale = max(scale, 0.1f);
	}
	else if(key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		rotate(0, 10);
	}
	else if(key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		rotate(0, -10);
	}
	else if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		samplingRate += 2;
		samplingRate = min(samplingRate, 80);
		initVBO();
	}
	else if(key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		samplingRate -= 2;
		samplingRate = max(samplingRate, 2);
		initVBO();
	}
	else if(key == GLFW_KEY_ENTER && action == GLFW_PRESS){
		if(mode){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		mode = !mode;
	}
}

int main(int argc, char** argv)
{
	GLFWwindow* window;
	if (!glfwInit())
	{
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 800, height = 600;
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
		return EXIT_FAILURE;
	}

	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER));
	strcat(rendererInfo, " - ");
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION));
	glfwSetWindowTitle(window, rendererInfo);

	if(!ParseInput(argv[1])){
		cout << "Input can not be read";
	}
	glEnable(GL_DEPTH_TEST);

	initShaders();
	initUniform();
	initVBO();

	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height);
	setInitialUniforms();

	while (!glfwWindowShouldClose(window))
	{
		clearDisplay();
		display();
		drawModel();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

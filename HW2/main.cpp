#include "glHelper.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLuint skyProgram;
GLuint skyTexture;
GLuint skyVAO;
GLuint skyVBO;
GLint skyProjectionMatrixLoc;
GLint skyViewingMatrixLoc;
GLint skyCenterPosLoc;

GLuint surfProgram;
GLuint surfTex;
GLuint surfVAO;
GLuint surfVBO;
GLint surfProjectionMatrixLoc;
GLint surfViewingMatrixLoc;
GLint surfCenterPosLoc;

GLFWwindow* window;

unsigned int cVertexCount;
unsigned int cFaceCount;
GLint cProjectionMatrixLoc;
GLint cViewingMatrixLoc;
GLint cCamarePosLoc;
GLuint cModelingMatrixLoc;
GLuint cVAO;
GLuint cVBO;
GLuint cVertexAttribBuffer;
GLuint cIndexBuffer;
vector<Car_Vertex> carVertices;
vector<Car_Face> carFaces;
GLuint carProgram;

glm::mat4 viewMatrix;
glm::mat4 modelMatrix;

GLuint sProgram;
GLint sProjectionMatrixLoc;
GLint sViewingMatrixLoc;
GLint sCenterPosLoc;
GLuint sVAO;
GLuint sVBO;
GLuint sVertexAttribBuffer;
GLuint sIndexBuffer;
unsigned int sVertexCount;
unsigned int sFaceCount;
vector<Structure_Vertex> structureVertices;
vector<Structure_Face> structureFaces;

float speed = 0;
float angle = 0;
float maxSpeed = 1;

glm::vec3 direction = glm::vec3(-1, 0, 0);
glm::vec3 cameraDirections[] = {glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, -1)};
glm::vec3 cameraUP[] = {glm::vec3(0, 1, 0), glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)};  

glm::vec3 centerPos = glm::vec3(0, 0, 0);
glm::vec3 cameraPos;

GLuint frameBuffer;
GLuint envirText;
GLuint renderBuffer;

void PressedKeyActionInst(int key){
	switch(key){
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_TAB:
			cout << "speed:" << speed << " angle:" << angle << " centerPos:" << centerPos.x << "," << centerPos.y << "," << centerPos.z << endl;
			break;
		case GLFW_KEY_SPACE:
			speed = 0.0f;
			break;
		case GLFW_KEY_Q:
			direction = glm::vec3(0, 0, -1);
			break;
		case GLFW_KEY_E:
			direction = glm::vec3(0, 0, 1);
			break;
		case GLFW_KEY_R:
			direction = glm::vec3(1, 0, 0);
			break;
		case GLFW_KEY_T:
			direction = glm::vec3(-1, 0, 0);
			break;
		default:
			break;
	}
}

void HoldKeyActionInst(int key) {
	switch(key){
		case GLFW_KEY_W:
			speed = min(speed + 0.2f, maxSpeed);
			break;
		case GLFW_KEY_S:
			speed = max(speed - 0.2f, -maxSpeed);
			break;
		case GLFW_KEY_A:
			angle -= 5;
			break;
		case GLFW_KEY_D:
			angle += 5;
			break;
		default:
			break;
	}
}

void ReleasedKeyActionInst(int key) {
	switch(key){
		
		default:
			break;
	}
}

void setupEnvir(){
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &envirText);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envirText);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	for (int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envirText, 0);
	}

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



bool ParseObj(const string& fileName, glm::vec3 ambiant_color, glm::vec3 reflective_color)
{
	fstream myfile;

	// Open the input
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open())
	{
		string curLine;
		vector<Vertex> gVertices;
		vector<Normal> gNormals;
		vector<Face> gFaces;
		glm::vec4 amb = glm::vec4(ambiant_color.r / 255.0f, ambiant_color.g/255.0f, ambiant_color.b/255.0f, 1.0f);
		glm::vec4 ref = glm::vec4(reflective_color.r / 255.0f, reflective_color.g/255.0f, reflective_color.b/255.0f, 1.0f); 
		unsigned int startPoint = carVertices.size();

		while (getline(myfile, curLine))
		{
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			string tmp;

			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 't') // texture
					{
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
					}
					else if (curLine[1] == 'n') // normal
					{
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						gNormals.push_back(Normal(c1, c2, c3));
					}
					else // vertex
					{
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						gVertices.push_back(Vertex(c1, c2, c3));
					}
				}
				else if (curLine[0] == 'f') // face
				{
					str >> tmp; // consume "f"
					char c;
					unsigned int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); // a limitation for now

					carFaces.push_back(Car_Face(vIndex[0] + startPoint -1, vIndex[1] + startPoint -1,vIndex[2] + startPoint -1));
				}
			}
		}

		myfile.close();
		for(unsigned int i = 0; i < gVertices.size(); i++){
			carVertices.push_back(Car_Vertex(glm::vec3(gVertices[i].x, gVertices[i].y, gVertices[i].z), glm::vec3(gNormals[i].x, gNormals[i].y, gNormals[i].z), glm::vec3(amb), glm::vec3(ref)));
		}

	}
	else
	{
		cout << "read error" << endl;
		return false;
	}
	return true;
}

void initCarVAO(){
	cVertexCount = carVertices.size();
	cFaceCount = carFaces.size();
	
	glGenVertexArrays(1, &(cVAO));
	glGenBuffers(1, &(cVBO));
	glBindVertexArray(cVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cVBO);

	float vertexData[cFaceCount * 36];

	cout << cVertexCount << "," << cFaceCount << endl;
	
	for(unsigned int i= 0; i < cFaceCount; i++){
		Car_Face f = carFaces[i];
		if(f.x >= carVertices.size() ||f.y >= carVertices.size() || f.z >= carVertices.size()){
			cout << f.x << "," << f.y << "," << f.z <<  " not found at " << i << endl;
		}
		Car_Vertex vert1 = carVertices[f.x];
		Car_Vertex vert2 = carVertices[f.y];
		Car_Vertex vert3 = carVertices[f.z];
		
		vertexData[36 * i] = vert1.position.x;
		vertexData[36 * i + 1] = vert1.position.y;
		vertexData[36 * i + 2] = vert1.position.z;

		vertexData[36 * i + 3] = vert1.normal.x;
		vertexData[36 * i + 4] = vert1.normal.y;
		vertexData[36 * i + 5] = vert1.normal.z;

		vertexData[36 * i + 6] = vert1.ambiant_color.x;
		vertexData[36 * i + 7] = vert1.ambiant_color.y;
		vertexData[36 * i + 8] = vert1.ambiant_color.z;

		vertexData[36 * i + 9] = vert1.reflective_color.x;
		vertexData[36 * i + 10] = vert1.reflective_color.y;
		vertexData[36 * i + 11] = vert1.reflective_color.z;

		vertexData[36 * i + 12] = vert2.position.x;
		vertexData[36 * i + 13] = vert2.position.y;
		vertexData[36 * i + 14] = vert2.position.z;

		vertexData[36 * i + 15] = vert2.normal.x;
		vertexData[36 * i + 16] = vert2.normal.y;
		vertexData[36 * i + 17] = vert2.normal.z;

		vertexData[36 * i + 18] = vert2.ambiant_color.x;
		vertexData[36 * i + 19] = vert2.ambiant_color.y;
		vertexData[36 * i + 20] = vert2.ambiant_color.z;

		vertexData[36 * i + 21] = vert2.reflective_color.x;
		vertexData[36 * i + 22] = vert2.reflective_color.y;
		vertexData[36 * i + 23] = vert2.reflective_color.z;

		vertexData[36 * i + 24] = vert3.position.x;
		vertexData[36 * i + 25] = vert3.position.y;
		vertexData[36 * i + 26] = vert3.position.z;

		vertexData[36 * i + 27] = vert3.normal.x;
		vertexData[36 * i + 28] = vert3.normal.y;
		vertexData[36 * i + 29] = vert3.normal.z;

		vertexData[36 * i + 30] = vert3.ambiant_color.x;
		vertexData[36 * i + 31] = vert3.ambiant_color.y;
		vertexData[36 * i + 32] = vert3.ambiant_color.z;

		vertexData[36 * i + 33] = vert3.reflective_color.x;
		vertexData[36 * i + 34] = vert3.reflective_color.y;
		vertexData[36 * i + 35] = vert3.reflective_color.z;

		
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
	glBindVertexArray(0);
	cout << "finished initCarVAO" << endl;
}

void setupCar(string carFolder){
	carProgram = initShader("car_vert.glsl", "car_frag.glsl");
	cCamarePosLoc = glGetUniformLocation(carProgram, "cameraPos");
	cModelingMatrixLoc = glGetUniformLocation(carProgram, "model");
	cViewingMatrixLoc = glGetUniformLocation(carProgram, "view");
	cProjectionMatrixLoc = glGetUniformLocation(carProgram, "projection");
	ParseObj(carFolder + "cybertruck_tires.obj", getColor(COLOR::BLACK), getColor(COLOR::BLACK));
	ParseObj(carFolder + "cybertruck_body.obj", getColor(COLOR::BLACK), getColor(COLOR::SILVER));
	ParseObj(carFolder + "cybertruck_tires.obj", getColor(COLOR::BLACK), getColor(COLOR::AQUA));
	initCarVAO();
}

void displayCar(){
	glBindTexture(GL_TEXTURE_CUBE_MAP, envirText);
	glUseProgram(carProgram);
	glBindVertexArray(cVAO);
	glUniform3fv(cCamarePosLoc, 1, glm::value_ptr(cameraPos));
	glUniformMatrix4fv(cProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(cViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(cModelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glBindBuffer(GL_ARRAY_BUFFER, cVBO);
	glDrawArrays(GL_TRIANGLES, 0, cFaceCount * 3);
	glBindVertexArray(0);
}

void displaySkyBox(){
	
	glUseProgram(skyProgram);
	glUniform3fv(skyCenterPosLoc, 1, glm::value_ptr(centerPos));
	glUniformMatrix4fv(skyProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(skyViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glBindVertexArray(skyVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void setupSkybox(string textureFolder){
	skyTexture = createCubeMap(textureFolder + "right.jpg", textureFolder + "left.jpg", textureFolder + "top.jpg", textureFolder +"bottom.jpg", textureFolder +"front.jpg", textureFolder + "back.jpg");
	skyProgram = initShader("skybox_vert.glsl", "skybox_frag.glsl");
	skyCenterPosLoc = glGetUniformLocation(skyProgram, "centerPos");
	skyProjectionMatrixLoc = glGetUniformLocation(skyProgram, "projectionMatrix");
	skyViewingMatrixLoc = glGetUniformLocation(skyProgram, "viewMatrix");
	glGenVertexArrays(1, &(skyVAO));
	glGenBuffers(1, &(skyVBO));
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	float skyboxVertices[] = {          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void displaySurface(){
	glUseProgram(surfProgram);
	glBindTexture(GL_TEXTURE_2D, surfTex);
	glBindVertexArray(surfVAO);
	glUniform3fv(surfCenterPosLoc, 1, glm::value_ptr(centerPos));
	glUniformMatrix4fv(surfProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(surfViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}


void setupSurface(){
	surfProgram = initShader("surface_vert.glsl", "surface_frag.glsl");
	surfTex = createTexture("hw2_support_files/ground_texture_sand.jpg");
	surfCenterPosLoc = glGetUniformLocation(surfProgram, "centerPos");
	surfProjectionMatrixLoc = glGetUniformLocation(surfProgram, "projectionMatrix");
	surfViewingMatrixLoc = glGetUniformLocation(surfProgram, "viewMatrix");
	glGenVertexArrays(1, &(surfVAO));
	glGenBuffers(1, &(surfVBO));
	glBindVertexArray(surfVAO);
	glBindBuffer(GL_ARRAY_BUFFER, surfVBO);
	float surfaceVertices[] = {          
        -200.0f, -1.0f, -200.0f, -1, -1,
        200.0f, -1.0f, 200.0f, 1, 1,
		-200.0f, -1.0f, 200.0f, -1, 1,
        -200.0f, -1.0f, -200.0f, -1, -1,
		200.0f, -1.0f, -200.0f, 1, -1,
        200.0f, -1.0f, 200.0f, 1, 1
    };

	glBufferData(GL_ARRAY_BUFFER, sizeof(surfaceVertices), &surfaceVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
}

void initStructureVAO(){
	sVertexCount = structureVertices.size();
	sFaceCount = structureFaces.size();
	
	glGenVertexArrays(1, &(sVAO));
	glGenBuffers(1, &(sVBO));
	glBindVertexArray(sVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sVBO);

	float vertexData[sFaceCount * 18];
	
	for(unsigned int i= 0; i < sFaceCount; i++){
		Structure_Face f = structureFaces[i];
		Structure_Vertex vert1 = structureVertices[f.x];
		Structure_Vertex vert2 = structureVertices[f.y];
		Structure_Vertex vert3 = structureVertices[f.z];
		vertexData[18 * i] = vert1.position.x;
		vertexData[18 * i + 1] = vert1.position.y;
		vertexData[18 * i + 2] = vert1.position.z;

		vertexData[18 * i + 3] = vert1.ambiant_color.x;
		vertexData[18 * i + 4] = vert1.ambiant_color.y;
		vertexData[18 * i + 5] = vert1.ambiant_color.z;

		vertexData[18 * i + 6] = vert2.position.x;
		vertexData[18 * i + 7] = vert2.position.y;
		vertexData[18 * i + 8] = vert2.position.z;

		vertexData[18 * i + 9] = vert2.ambiant_color.x;
		vertexData[18 * i + 10] = vert2.ambiant_color.y;
		vertexData[18 * i + 11] = vert2.ambiant_color.z;

		vertexData[18 * i + 12] = vert3.position.x;
		vertexData[18 * i + 13] = vert3.position.y;
		vertexData[18 * i + 14] = vert3.position.z;

		vertexData[18 * i + 15] = vert3.ambiant_color.x;
		vertexData[18 * i + 16] = vert3.ambiant_color.y;
		vertexData[18 * i + 17] = vert3.ambiant_color.z;
	}
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), &vertexData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);
}	


bool readOBJFile(const string& fileName, glm::vec3 color, glm::vec3 scale, glm::vec3 position)
{
	fstream myfile;

	// Open the input 
	myfile.open(fileName.c_str(), ios::in);

	if (myfile.is_open())
	{
		string curLine;
		unsigned int startPoint = structureVertices.size();
		glm::mat4 tMat = glm::mat4(scale.x, 0, 0, position.x, 
			0, scale.y, 0, position.y,
			0, 0, scale.z, position.z,
			0, 0, 0, 1);
		glm::vec3 amb = glm::vec3(color.r / 255.0f, color.g/255.0f, color.b/255.0f); 
		while (getline(myfile, curLine))
		{
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			string tmp;

			if (curLine.length() >= 2)
			{
				if (curLine[0] == 'v')
				{
					if (curLine[1] == 'n') // normal
					{
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
					}
					else // vertex
					{
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						glm::vec4 pos = glm::vec4(c1 * scale.x + position.x, c2 * scale.y + position.y, c3 * scale.z + position.z, 1.0);
						structureVertices.push_back(Structure_Vertex(pos, amb));
					}
				}
				else if (curLine[0] == 'f') // face
				{
					str >> tmp; // consume "f"
					char c;
					int vIndex[3], nIndex[3];
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); // a limitation for now

					structureFaces.push_back(Structure_Face(vIndex[0] + startPoint -1, vIndex[1] + startPoint -1,vIndex[2] + startPoint -1));
				}
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

void setupStructures(string modelFolder){
	sProgram = initShader("structure_vert.glsl", "structure_frag.glsl");
	sCenterPosLoc = glGetUniformLocation(sProgram, "centerPos");
	sProjectionMatrixLoc = glGetUniformLocation(sProgram, "projectionMatrix");
	sViewingMatrixLoc = glGetUniformLocation(sProgram, "viewMatrix");

	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::RED), glm::vec3(1, 1, 1), glm::vec3(7.0f, 0.0f, 0.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::BLUE), glm::vec3(1, 1, 1), glm::vec3(-7.0f, 0.0f, 0.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::WHITE), glm::vec3(1, 1, 1), glm::vec3(0.0f, 0.0f, 7.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::BLACK), glm::vec3(1, 1, 1), glm::vec3(0.0f, 0.0f, -7.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::AQUA), glm::vec3(1, 1, 1), glm::vec3(7.0f, 0.0f, 7.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::LIME), glm::vec3(1, 1, 1), glm::vec3(7.0f, 0.0f, -7.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::FUCHSIA), glm::vec3(1, 1, 1), glm::vec3(-7.0f, 0.0f, 7.0f));
	readOBJFile(modelFolder + "cube.obj", getColor(COLOR::TEAL), glm::vec3(1, 1, 1), glm::vec3(-7.0f, 0.0f, -7.0f));
	initStructureVAO();
}

void displayStructures(){
	glUseProgram(sProgram);
	glBindVertexArray(sVAO);
	glUniform3fv(sCenterPosLoc, 1, glm::value_ptr(centerPos));
	glUniformMatrix4fv(sProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(sViewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glBindBuffer(GL_ARRAY_BUFFER, sVBO);
	glDrawArrays(GL_TRIANGLES, 0, sFaceCount * 3);
	glBindVertexArray(0);
}

void updatePos(){
	glm::mat4 rot = glm::rotate<float>(glm::mat4(1.0), (-angle / 180.0) * M_PI, glm::vec3(0.0, 1.0, 0.0));
	glm::vec4 forward = glm::vec4(10.0f, 0.0f, 0.0f, 1.0f) * rot;
	centerPos += speed * glm::normalize(glm::vec3(-forward));
	cameraPos = centerPos - glm::vec3(glm::vec4(direction, 1.0f) * rot) + glm::vec3(0, 0.2, 0);
	modelMatrix = glm::translate(glm::mat4(1.0), centerPos - glm::vec3(0.0, 0.8, 0.0)) * glm::rotate<float>(glm::mat4(1.0), ((angle - 90.0)/ 180.0) * M_PI, glm::vec3(0.0, 1.0, 0.0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
	//viewMatrixCar = glm::lookAt(cameraPos, centerPos, glm::vec3(0, -1, 0));
}

int main()
{
	// PROPERTIES
	int width = 600;
	int height = 600;
	string textureFolder = "hw2_support_files/skybox_texture_sea/";
	string carFolder = "hw2_support_files/obj/cybertruck/";
	string modelFolder = "hw2_support_files/obj/";

	window = InitilizeWindow(width, height);
	initKeyboard(&HoldKeyActionInst, &PressedKeyActionInst, &ReleasedKeyActionInst);

	glfwSetKeyCallback(window, ParseKeyboardEvent);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height);
	
	updatePos();
	
	setupSurface();
	setupSkybox(textureFolder);
	setupStructures(modelFolder);

	setupEnvir();
	setupCar(carFolder);
	glEnable(GL_DEPTH_TEST);
	

	while (!glfwWindowShouldClose(window))
	{
		setWindow(window, 600, 600, 45);
		for(unsigned int i = 0; i < 6; i++){
			viewMatrix = glm::lookAt(centerPos - cameraDirections[i], centerPos, -cameraUP[i]);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			
			glDisable(GL_DEPTH_TEST);
			displaySkyBox();
			glEnable(GL_DEPTH_TEST);

			displayStructures(); 
			displaySurface();
			

		}
		setWindow(window, gWidth, gHeight, 90);
		viewMatrix = glm::lookAt(cameraPos, centerPos, glm::vec3(0, 1, 0));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		displaySkyBox();
		glEnable(GL_DEPTH_TEST);

		displayStructures();
		displaySurface();
		displayCar();

		glfwSwapBuffers(window);
		glfwPollEvents();
		ParseHoldKeys();
		updatePos();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

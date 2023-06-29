#include "glHelper.hpp"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

GLFWwindow* window;

glm::vec3 upVec = glm::vec3(0, 1, 0);
glm::vec3 gazeVec = glm::vec3(1, 0, 0);
glm::vec3 rightVec = glm::vec3(0, 0, 1);

glm::quat currentRotation = glm::quat(glm::vec3(0, 0, 0));

glm::vec3 position = glm::vec3(0, 0, 0);

bool positionChanged = true;
bool propertiesChanged = true;

float speed = 0.0f;
float maxSpeed = 10.0f;

float deltaTime = 0.0f;
float iFrameTime = 0.0f;

float acceleration = 1.0f;

float anglePerSecond = 10.0f;

bool renderCloud = true;

glm::mat4 viewMatrix;
glm::mat4 modelMatrix = glm::mat4(1.0f);

GLuint program;
GLuint cubemap;
GLuint VAO;
GLuint VBO;

GLint projectionMatrixLoc;
GLint viewingMatrixLoc;
GLint modelingMatrixLoc;

GLint cameraPositionLoc;

GLint rayMarchDistanceLoc;
GLint rayMarchCountLoc;

GLint turbulanceCountLoc;
GLint maxYLoc;
GLint minYLoc;

GLint gaussEnabledLoc;

unsigned int gaussEnabled = 0;

float rayMarchDistance = 0.20f;

float maxY = +10.0f;
float minY = -10.0f;

int rayMarchCount = 6;
int turbulanceCount = 8;

void Turn(glm::vec3 baseAxis, float angle) {
	glm::vec3 axis = currentRotation * baseAxis;
	currentRotation = glm::angleAxis(angle, axis) * currentRotation;
	positionChanged = true;
}

void Roll(float angle) {
	Turn(gazeVec, angle);
}

void Yaw(float angle) {
	Turn(upVec, angle);
}

void Pitch(float angle) {
	Turn(rightVec, angle);
}

void printVector(glm::vec3 vec) {
	cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";

}

void PressedKeyActionInst(int key) {
	switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_Z:
			rayMarchCount -= 1;
			propertiesChanged = true;
			break;
		case GLFW_KEY_X:
			rayMarchCount += 1;
			propertiesChanged = true;
			break;
		case GLFW_KEY_C:
			turbulanceCount -= 1;
			propertiesChanged = true;
			break;
		case GLFW_KEY_V:
			turbulanceCount += 1;
			propertiesChanged = true;
			break;
		case GLFW_KEY_SPACE:
			speed = 0;
			positionChanged = true;
			break;
		case GLFW_KEY_M:
			currentRotation = glm::quat(glm::vec3(0, 0, 0));
			positionChanged = true;
			break;
		case GLFW_KEY_T:
			renderCloud = !renderCloud;
			propertiesChanged = true;
			break;
		case GLFW_KEY_TAB:
			cout << "speed: " << speed << "\t position: "; printVector(position); cout << endl;
			cout << "up: "; printVector(currentRotation * upVec); cout << "\t gaze: "; printVector(currentRotation * gazeVec); cout << "\t right: "; printVector(currentRotation * rightVec); cout << endl;
			cout << "cloudY: (" << minY << ", " << maxY << ")"  << endl;
			cout << "rayMarchCount: " << rayMarchCount << "\t rayMarchDistance: " << rayMarchDistance << "\t turbulenceCount: " << turbulanceCount << endl;
			cout << endl;
			break;
		default:
			break;
	}
}

void HoldKeyActionInst(int key) {
	switch (key) {
		case GLFW_KEY_W:
			speed = min(maxSpeed, speed + (acceleration * deltaTime));
			break;
		case GLFW_KEY_S:
			speed = max(-maxSpeed, speed - (acceleration * deltaTime));
			break;
		case GLFW_KEY_A:
			Roll(anglePerSecond * deltaTime);
			break;
		case GLFW_KEY_D:
			Roll(-anglePerSecond * deltaTime);
			break;
		case GLFW_KEY_Q:
			Yaw(anglePerSecond * deltaTime);
			break;
		case GLFW_KEY_E:
			Yaw(-anglePerSecond * deltaTime);
			break;
		case GLFW_KEY_U:
			Pitch(anglePerSecond * deltaTime);
			break;
		case GLFW_KEY_J:
			Pitch(-anglePerSecond * deltaTime);
			break;
		case GLFW_KEY_B:
			rayMarchDistance -= deltaTime;
			propertiesChanged = true;
			break;
		case GLFW_KEY_N:
			rayMarchDistance += deltaTime;
			propertiesChanged = true;
			break;
		default:
			break;
	}
}

void ReleasedKeyActionInst(int key) {
	switch (key) {
		default:
			break;
	}
}

void InitProgram() {
	program = initShader("quad_vert.glsl", "quad_frag.glsl");
	projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
	viewingMatrixLoc = glGetUniformLocation(program, "viewingMatrix");;
	modelingMatrixLoc = glGetUniformLocation(program, "modelingMatrix");

	cameraPositionLoc = glGetUniformLocation(program, "cameraPosition");

	rayMarchDistanceLoc = glGetUniformLocation(program, "rayMarchDistance");
	rayMarchCountLoc = glGetUniformLocation(program, "rayMarchCount");

	turbulanceCountLoc = glGetUniformLocation(program, "turbulanceCount");
	maxYLoc = glGetUniformLocation(program, "maxY");
	minYLoc = glGetUniformLocation(program, "minY");

	gaussEnabledLoc = glGetUniformLocation(program, "gaussEnabled");
}

void InitQuad() {

	glGenVertexArrays(1, &(VAO));
	glGenBuffers(1, &(VBO));
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	float quadVertices[] = {
		 2.0f, -10.0f, -10.0f,
		 2.0f, -10.0f,  10.0f,
		 2.0f,  10.0f,  10.0f,
		 2.0f,  10.0f,  10.0f,
		 2.0f,  10.0f, -10.0f,
		 2.0f, -10.0f, -10.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

void SetUniforms() {
	glUseProgram(program);
	if (positionChanged = true || abs(speed) > 0.00001f) {
		glUniform3fv(cameraPositionLoc, 1, glm::value_ptr(position));
		glUniformMatrix4fv(viewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		positionChanged = false;
	}
	

	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	
	if (propertiesChanged) {
		glUniform1f(rayMarchDistanceLoc, rayMarchDistance);
		if(renderCloud)
			glUniform1i(rayMarchCountLoc, rayMarchCount);
		else
			glUniform1i(rayMarchCountLoc, 0);

		glUniform1i(turbulanceCountLoc, turbulanceCount);

		glUniform1f(maxYLoc, maxY);
		glUniform1f(minYLoc, minY);

		glUniform1ui(gaussEnabledLoc, gaussEnabled);

		propertiesChanged = false;
	}
}

void DrawQuad() {
	glUseProgram(program);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void UpdatePosition() {
	float dist = speed * deltaTime;
	glm::vec3 gaze = currentRotation * gazeVec;
	glm::vec3 movement = gaze * dist;
	glm::mat4 trans = glm::translate(glm::mat4(1.0f), movement);
	position = glm::vec3(trans * glm::vec4(position, 1.0f));
	modelMatrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(currentRotation);
	viewMatrix = glm::lookAt(position, glm::vec3(modelMatrix * glm::vec4(gazeVec, 1.0f)), currentRotation * upVec);
}

int main() {
	int width = 800;
	int height = 600;

	window = InitilizeWindow(width, height);
	initKeyboard(&HoldKeyActionInst, &PressedKeyActionInst, &ReleasedKeyActionInst);

	glfwSetKeyCallback(window, ParseKeyboardEvent);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height);

	glEnable(GL_DEPTH_TEST);

	InitProgram();
	InitQuad();
	cubemap = createCubeMap("cubemap/right.jpg", "cubemap/left.jpg", "cubemap/top.jpg", "cubemap/bottom.jpg", "cubemap/front.jpg", "cubemap/back.jpg");


	while (!glfwWindowShouldClose(window))
	{
		deltaTime = -((iFrameTime)-(iFrameTime = glfwGetTime()));
		clearDisplay();
		SetUniforms();
		DrawQuad();


		glfwSwapBuffers(window);
		glfwPollEvents();
		ParseHoldKeys();
		UpdatePosition();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

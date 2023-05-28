#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

enum class COLOR {
	WHITE, SILVER, GRAY, BLACK, RED, MAROON, YELLOW, OLIVE, LIME, GREEN, AQUA, TEAL, BLUE, NAVY, FUCHSIA, PURPLE
};

glm::vec3 getColor(COLOR c){
	switch (c)
	{
	case COLOR::WHITE:
		return glm::vec3(255.0f, 255.0f, 255.0f);
	case COLOR::SILVER:
		return glm::vec3(192.0f, 192.0f, 192.0f);
	case COLOR::GRAY:
		return glm::vec3(128.0f, 128.0f, 128.0f);
	case COLOR::BLACK:
		return glm::vec3(0.0f, 0.0f, 0.0f);
	case COLOR::RED:
		return glm::vec3(255.0f, 0.0f, 0.0f);
	case COLOR::MAROON:
		return glm::vec3(128.0f, 0.0f, 0.0f);
	case COLOR::YELLOW:
		return glm::vec3(255.0f, 255.0f, 0.0f);
	case COLOR::OLIVE:
		return glm::vec3(128.0f, 128.0f, 0.0f);
	case COLOR::LIME:
		return glm::vec3(0.0f, 255.0f, 0.0f);
	case COLOR::GREEN:
		return glm::vec3(0.0f, 128.0f, 0.0f);
	case COLOR::AQUA:
		return glm::vec3(0.0f, 255.0f, 255.0f);
	case COLOR::TEAL:
		return glm::vec3(0.0f, 128.0f, 128.0f);
	case COLOR::BLUE:
		return glm::vec3(0.0f, 0.0f, 255.0f);
	case COLOR::NAVY:
		return glm::vec3(0.0f, 0.0f, 128.0f);
	case COLOR::FUCHSIA:
		return glm::vec3(255.0f, 0.0f, 255.0f);
	case COLOR::PURPLE:
		return glm::vec3(128.0f, 0.0f, 128.0f);
	default:
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
}
struct Vertex
{
	Vertex(GLfloat inX = 0, GLfloat inY = 0, GLfloat inZ = 0) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};
struct Texture
{
	Texture(GLfloat inU = 0, GLfloat inV = 0) : u(inU), v(inV) { }
	GLfloat u, v;
};
struct Normal
{
	Normal(GLfloat inX = 0, GLfloat inY = 0, GLfloat inZ = 0) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};
struct Face
{
	Face(unsigned int v[], unsigned int t[], unsigned int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};
struct Structure_Vertex{
	Structure_Vertex(glm::vec3 pos, glm::vec3 amb) : position(pos), ambiant_color(amb) {}
	glm::vec3 position;
	glm::vec3 ambiant_color;
};
struct Structure_Face{
	Structure_Face(GLuint inX = 0, GLuint inY = 0, GLuint inZ = 0) : x(inX), y(inY), z(inZ) { }
	GLuint x, y, z;
};
struct Car_Vertex{
    Car_Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec3 amb, glm::vec3 ref) : position(pos), normal(norm), ambiant_color(amb), reflective_color(ref) {}
    glm::vec3 position;
    glm::vec3 normal;
	glm::vec3 ambiant_color;
    glm::vec3 reflective_color;
};
struct Car_Face{
	Car_Face(GLuint inX = 0, GLuint inY = 0, GLuint inZ = 0) : x(inX), y(inY), z(inZ) { }
	GLuint x, y, z;
};

bool* key_held_down;
void (*HoldKeyAction)(int);
void (*PressedKeyAction)(int);
void (*ReleasedKeyAction)(int);

void initKeyboard(void (*hold)(int), void (*press)(int), void (*release)(int)) {
	HoldKeyAction = hold;
	PressedKeyAction = press;
	ReleasedKeyAction = release;
	key_held_down = new bool[GLFW_KEY_LAST + 1];
	for (unsigned int i = 0; i <= GLFW_KEY_LAST; i++) {
		key_held_down[i] = false;
	}
}

void KeyPressed(int key) {
	key_held_down[key] = true;
	PressedKeyAction(key);
}

void KeyReleased(int key) {
	key_held_down[key] = false;
	ReleasedKeyAction(key);
}

void ParseKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		KeyPressed(key);
	}
	else if (action == GLFW_RELEASE) {
		KeyReleased(key);
	}
}

void ParseHoldKeys() {
	for (unsigned int i = 0; i <= GLFW_KEY_LAST; i++) {
		if (key_held_down[i])
			HoldKeyAction(i);
	}
}
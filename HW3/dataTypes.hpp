#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace std;

enum class COLOR {
	WHITE, SILVER, GRAY, BLACK, RED, MAROON, YELLOW, OLIVE, LIME, GREEN, AQUA, TEAL, BLUE, NAVY, FUCHSIA, PURPLE
};

glm::vec3 getColor(COLOR c) {
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
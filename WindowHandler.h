#pragma once
#include<GLFW/glfw3.h>

#define WH_NONE 0
#define WH_CLEAR 1
#define WH_ZOOM_OUT 2
#define WH_ZOOM_IN 3
#define WH_ZOOM_END 4
#define WH_ITER_UP 5
#define WH_ITER_DOWN 6
#define WH_FREE_RUN 7
#define WH_COPY_COORDINATES 8
#define WH_PASTE_COORDINATES 9

class WindowHandler
{
private:
	bool quitKeyPressed = false;
	int keyAction = WH_NONE;
public:
	double xPos;
	double yPos;

	WindowHandler(GLFWwindow* window);
	void keyEvent(int key, int scancode, int action, int mods);
	void mouseButtonEvent(int button, int action, int mods);

	int getKeyAction();
	bool quitPressed();
};

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	WindowHandler* handler = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
	if (handler)
	{
		handler->keyEvent(key, scancode, action, mods);
	}
}

static void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	WindowHandler* handler = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
	if (handler)
	{
		handler->mouseButtonEvent(button, action, mods);
	}
}

static void glfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	WindowHandler* handler = reinterpret_cast<WindowHandler*>(glfwGetWindowUserPointer(window));
	if (handler)
	{
		handler->xPos = xpos;
		handler->yPos = ypos;
	}
}

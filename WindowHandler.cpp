#include "WindowHandler.h"

WindowHandler::WindowHandler(GLFWwindow* window) 
{
	glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));
	glfwSetKeyCallback(window, glfwKeyCallback);
	glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
	glfwSetCursorPosCallback(window, glfwCursorPosCallback);
}

void WindowHandler::keyEvent(int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS)
	{
		return;
	}

	switch (key)
	{
	case GLFW_KEY_Q: 
		quitKeyPressed = true;
		break;
	case GLFW_KEY_W:
		keyAction = WH_CLEAR;
		break;
	case GLFW_KEY_I:
		keyAction = WH_ITER_UP;
		break;
	case GLFW_KEY_O:
		keyAction = WH_ITER_DOWN;
		break;
	case GLFW_KEY_F:
		keyAction = WH_FREE_RUN;
		break;
	case GLFW_KEY_C:
		keyAction = WH_COPY_COORDINATES;
		break;
	case GLFW_KEY_V:
		keyAction = WH_PASTE_COORDINATES;
		break;
		//case GLFW_KEY_UP:
	//	keyAction = WH_ZOOM_IN;
	//	break;
	//case GLFW_KEY_DOWN:
	//	keyAction = WH_ZOOM_OUT;
	//	break;
	}
}

void WindowHandler::mouseButtonEvent(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		keyAction = action == 1 ? WH_ZOOM_IN : WH_ZOOM_END;
	}
	else if (button == GLFW_MOUSE_BUTTON_2)
	{
		keyAction = action == 1 ? WH_ZOOM_OUT : WH_ZOOM_END;
	}
}

bool WindowHandler::quitPressed()
{
	return quitKeyPressed;
}

int WindowHandler::getKeyAction()
{
	int lastAction = keyAction;
	keyAction = WH_NONE;

	return lastAction;
}

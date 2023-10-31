#include "Camera.h"
#include <iostream>
#include "glm/gtx/rotate_vector.hpp"

namespace Graphics {
	namespace {
		void firstPersonFlying(Camera* cam, GLFWwindow* window, int width, int height, bool focused)
		{
			glm::vec3 mov(0.f);
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				mov.z += 1;
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				mov.z -= 1;
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
				mov.x -= 1;
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
				mov.x += 1;
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				mov.y += 1;
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				mov.y -= 1;
			cam->transform.translate(mov * cam->control.speed);



			if (focused && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				double curX, curY;
#ifdef __EMSCRIPTEN__
				glfwGetCursorPos(window, &curX, &curY);
				if (cam->control.firstRotate)
				{
					cam->control.cursorPos.x = curX;
					cam->control.cursorPos.y = curY;
					cam->control.firstRotate = 0;
				}
				glm::vec3 dir(curX - cam->control.cursorPos.x, curY - cam->control.cursorPos.y, 0),
					axis(0);
				cam->control.cursorPos.x = curX;
				cam->control.cursorPos.y = curY;
#else
				if (cam->control.firstRotate)
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					glfwSetCursorPos(window, width / 2, height / 2);
					cam->control.firstRotate = 0;
				}
				glfwGetCursorPos(window, &curX, &curY);

				glm::vec3 dir(curX - width / 2, curY - height / 2, 0),
					axis(0);
				glfwSetCursorPos(window, width / 2, height / 2);
#endif
				if (dir != axis)
				{
					axis = glm::normalize(glm::cross(cam->transform.orientation, cam->transform.up));
					cam->transform.rotate(-cam->control.sensitivity * dir.y, axis);
					cam->transform.rotate(-cam->control.sensitivity * dir.x, cam->transform.up);
				}
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				cam->control.firstRotate = 1;
			}
		}
		void nothing(Camera* cam, GLFWwindow* window, int width, int height, bool focused){}
	}

	Controller::Controller(ControllerType type)
		:speed(.1f), sensitivity(.01f), firstRotate(1)
	{
		switch (type)
		{
		case ControllerType::None:
			behavior = nothing;
			break;
		case ControllerType::FirstPersonFlying:
			behavior = firstPersonFlying;
			break;
		}
	}

	void Controller::setToFirstPersonFlying()
	{
		behavior = firstPersonFlying;
	}
	void Controller::setToNone()
	{
		behavior = nothing;
	}

	Camera::Camera(glm::vec3 start)
		:transform(start, glm::vec3(0, 0, -1))
	{}
	Camera::~Camera()
	{}

	glm::mat4 Camera::view() const
	{
		glm::vec3 tempPos = transform.position;
		glm::vec3 tempOr = transform.orientation;
		Transform* tempParent = transform.parent;
		while (tempParent)
		{
			tempPos += tempParent->position;
			tempOr += tempParent->orientation;
			tempParent = tempParent->parent;
		}
		return glm::lookAt(tempPos, tempPos + tempOr, transform.up) * glm::scale(transform.shape);
	}

	void Camera::getInput(GLFWwindow* window, int width, int height, bool focused)
	{
		control.behavior(this, window, width, height, focused);
	}
}
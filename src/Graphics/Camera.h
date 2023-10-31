#pragma once
#include <GLFW/glfw3.h>

#include "Transform.h"

namespace Graphics {
	class Camera;
	class Controller
	{
	public:
		enum class ControllerType
		{
			None,FirstPersonFlying
		};
		float speed, sensitivity;
		bool firstRotate;
#ifdef __EMSCRIPTEN__
		glm::vec2 cursorPos;
#endif
		void (*behavior)(Camera*, GLFWwindow*, int, int, bool);

		Controller(ControllerType type=ControllerType::FirstPersonFlying);

		void setToFirstPersonFlying();
		void setToNone();
	};
	class Camera
	{
	public:
		Transform transform;
		Controller control;
	public:
		Camera(glm::vec3 start = glm::vec3(0));
		~Camera();

		glm::mat4 view() const;	

		void getInput(GLFWwindow* window, int width, int height, bool focused);
	};
}
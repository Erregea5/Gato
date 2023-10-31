#pragma once

#include "Transform.h"
#include <vector>
class Object;

namespace Physics {

	class Collider : public Transform
	{
	public:
		Object* obj;
		enum class ColliderType
		{
			Sphere, Cube
		};
		ColliderType type;

		glm::vec3 getPosition();
		glm::vec3 getOrientation();

		void collisionResolution(Collider& collider);
	};

	void collisionDetection(std::vector<Object>& objList);
}
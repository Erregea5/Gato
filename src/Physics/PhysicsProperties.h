#pragma once
#include "Transform.h"

namespace Physics {
	class PhysicsProperties
	{
	public:
		glm::vec3 velocity;
		glm::vec3 acceleration;
		glm::vec3 angularVelocity;
		glm::vec3 angularAcceleration;
		float mass;
		float friction;
		float dt=.02f;
	public:
		void update() {
			velocity += acceleration * dt;
			angularVelocity += angularAcceleration * dt;
		}
		glm::vec3 dpos() {
			return velocity * dt;
		}
	};
}
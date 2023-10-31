#include "Collision.h"
#include "Object.h"

namespace Physics {
	namespace {
		float magnitude(glm::vec3 vec) {
			return vec.x * vec.x + vec.y * vec.y + vec.z + vec.z;
		}
	}

	glm::vec3 Collider::getPosition()
	{
		glm::vec3 forward = obj->transform.orientation;//glm::vec3(orientation.x, 0, orientation.z);
		glm::vec3 sideways(0.f);
		if (forward.x != 0 || forward.z != 0)
			sideways = glm::normalize(glm::cross(forward, up));
		else
			sideways = glm::normalize(glm::cross(forward, glm::vec3(1, 0, 0)));

		forward = glm::normalize(forward);
		//walk in direction of orientation
		return obj->transform.position + glm::mat3(sideways, glm::normalize(glm::cross(forward, sideways)), forward) * position;
	}

	glm::vec3 Collider::getOrientation()
	{
		glm::vec3 forward = obj->transform.orientation;//glm::vec3(orientation.x, 0, orientation.z);
		glm::vec3 sideways(0.f);
		if (forward.x != 0 || forward.z != 0)
			sideways = glm::normalize(glm::cross(forward, up));
		else
			sideways = glm::normalize(glm::cross(forward, glm::vec3(1, 0, 0)));

		forward = glm::normalize(forward);
		return glm::mat3(sideways, glm::cross(forward, sideways), forward) * orientation;
	}

	void Collider::collisionResolution(Collider& collider)
	{
		float dist = glm::distance(getPosition(), collider.getPosition());
		glm::vec3 dif = position - collider.position;
		if (type == ColliderType::Sphere)
		{
			if (collider.type == ColliderType::Sphere)
			{
				float max = shape.x > collider.shape.x ? shape.x : collider.shape.x;
				if (max > dist) {
					obj->transform.translate(dif);
					collider.obj->transform.translate(dif);
				}
			}
			else if (collider.type == ColliderType::Cube)
			{

			}
		}
	}


	void collisionDetection(std::vector<Object>& objList)
	{
		//lazy collision detection
		for (int i = 0;i < objList.size() - 1;i++)
			for (int u = i + 1;u < objList.size();u++)
				if (objList[i].isActive && objList[u].isActive);
					//for (auto& c1 : objList[i].colliders)
						//for (auto& c2 : objList[u].colliders)
							//c1.collisionResolution(c2);
	}
}
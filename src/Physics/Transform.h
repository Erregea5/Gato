#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

class Transform
{
public:
	glm::vec3 up = glm::vec3(0.f, 1.f, 0.f),
		position,
		orientation,
		shape;
	Transform* parent;
	std::vector<Transform*> children;
	std::vector<std::string> childrenNames;
public:
	Transform(glm::vec3 _position, glm::vec3 _orientation);
	Transform();
	~Transform();

	void translate(glm::vec3 direction);
	void setPosition(glm::vec3 newPosition);
	void rotate(float angle, glm::vec3 axis);
	void setOrientation(glm::vec3 newOrientation);
	void scale(float _scale);
	void scale(glm::vec3 _scale);
	void setScale(glm::vec3 newScale);

	void setParent(Transform& newParent);
	void removeParent();
	void addChild(const std::string& childName, Transform& newChild);
	void removeChild(const std::string& childName);
	void removeChild(Transform& newChild);
	virtual glm::mat4 view() const;
};
#include "Transform.h"

Transform::Transform(glm::vec3 _position, glm::vec3 _orientation)
	:position(_position), orientation(_orientation), shape(glm::vec3(1)), parent(0)
{}
Transform::Transform()
	:shape(glm::vec3(1)), parent(0)
{}
Transform::~Transform()
{
	if (parent)
	{
		for (int i = 0;i < children.size();i++)
			parent->addChild(childrenNames[i], *children[i]);
		parent->removeChild(*this);
	}
	else
		for (auto child : children)
			child->removeParent();
}

void Transform::translate(glm::vec3 direction)
{
	glm::vec3 forward = glm::vec3(orientation.x, 0, orientation.z);
	glm::vec3 sideways(0.f);
	if (forward.x != 0 || forward.z != 0)
	{
		sideways = glm::normalize(glm::cross(forward, up));
		forward = glm::normalize(forward);
	}

	position += glm::mat3(sideways, up, forward) * direction;
}
void Transform::setPosition(glm::vec3 newPosition)
{
	position = newPosition;
}
void Transform::rotate(float angle, glm::vec3 axis)
{
	glm::vec3 newOrientation = glm::rotate(orientation, angle, axis);
	setOrientation(newOrientation);
}
void Transform::setOrientation(glm::vec3 newOrientation)
{
	float error = .1f;
	if(glm::length(glm::cross(newOrientation,up))>error)
		orientation = newOrientation;
}

void Transform::scale(glm::vec3 _scale)
{
	shape += _scale;
}

void Transform::scale(float _scale)
{
	shape += glm::vec3(_scale);
}

void Transform::setScale(glm::vec3 newScale)
{
	shape = newScale;
}

void Transform::setParent(Transform& newParent)
{
	parent = &newParent;
}

void Transform::removeParent()
{
	parent = 0;
}

void Transform::addChild(const std::string& childName, Transform& newChild)
{
	childrenNames.push_back(childName);
	children.push_back(&newChild);
	newChild.setParent(*this);
}
void Transform::removeChild(const std::string& childName)
{
	int i;
	for (i = 0;i < childrenNames.size();i++) 
		if (childrenNames[i] == childName)
			break;
	if (i >= childrenNames.size())
		return;
	childrenNames[i] = childrenNames.back();
	children[i]->removeParent();
	children[i] = children.back();
	childrenNames.pop_back();
	children.pop_back();
}
void Transform::removeChild(Transform& newChild)
{
	int i;
	for (i = 0;i < children.size();i++)
		if (children[i] == &newChild)
			break;
	if (i >= children.size())
		return;
	childrenNames[i] = childrenNames.back();
	children[i]->removeParent();
	children[i] = children.back();
	childrenNames.pop_back();
	children.pop_back();
}
#include <glm/gtx/euler_angles.hpp>
glm::mat4 Transform::view() const
{
	glm::vec3 tempPos = position;
	glm::vec3 tempOr = orientation;
	glm::vec3 tempShape = shape;
	Transform* tempParent=parent;
	while (tempParent)
	{
		tempPos += tempParent->position;
		tempOr += tempParent->orientation;
		tempShape += tempParent->shape;
		tempParent = tempParent->parent;
	}
	return glm::translate(tempPos) * glm::eulerAngleXYZ(tempOr.x, tempOr.y, tempOr.z) * glm::scale(tempShape);
}
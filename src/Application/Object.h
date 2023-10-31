#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
//#include "Collision.h"
#include "PhysicsProperties.h"
#include "Source.h"

class Object
{
public:
	Transform transform;
	Audio::Source audio;
	Graphics::Mesh* mesh;
	Graphics::Shader* shader;
	Graphics::Texture* texture;

	//std::vector<Physics::Collider> colliders;
	Physics::PhysicsProperties physics;

	char defaults;//1s bit for shader, 2s bit for mesh
	bool isActive;
	bool isPhysics;
	bool hasShadow;

public:
	Object(Graphics::Mesh& m, Graphics::Shader& s, Graphics::Texture& t, glm::vec3 pos = glm::vec3(0));
	Object(Graphics::Mesh& m, Graphics::Shader& s, glm::vec3 pos = glm::vec3(0));
	Object(Graphics::Mesh& m, glm::vec3 pos = glm::vec3(0));
	Object(Graphics::Mesh::PreMade options, Graphics::Shader& s, Graphics::Texture& t, glm::vec3 pos= glm::vec3(0));
	Object(Graphics::Mesh::PreMade options, Graphics::Shader& s, glm::vec3 pos= glm::vec3(0));
	Object(Graphics::Mesh::PreMade options, glm::vec3 pos= glm::vec3(0));
	Object(glm::vec3 pos= glm::vec3(0));
	~Object();

	void bind() const;
	void unBind() const;

	void setShader(Graphics::Shader& newShader);
	void removeShader();
	void setTexture(Graphics::Texture& newTexture);
	void removeTexture();
	void setMesh(Graphics::Mesh& newMesh);
	void removeMesh();
	void setShadow(bool);

	void setCameraView(const glm::mat4& proj, const glm::mat4& camView) const;
	int getMeshIboCount() const { return mesh->getIboCount(); }

	Transform& getTransform();

	void physicsUpdate()
	{
		if (isPhysics&&isActive) {
			physics.update();
			transform.translate(physics.dpos());
		}
		audio.setPosition(transform.position);
		audio.setVelocity(physics.velocity);
	}
private:
	void constructor(glm::vec3 pos = glm::vec3(0));
};
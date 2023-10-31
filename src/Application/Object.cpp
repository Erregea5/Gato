#include "Object.h"
#include <iostream>

void Object::constructor(glm::vec3 pos)
{
	isActive = 1;
	transform.position = pos;
	transform.orientation = glm::vec3(0, 0, 1);
	hasShadow = true;
	if (shader)
	{
		shader->bind();
		if (texture)
		{
			shader->setTexture("u_Texture", *texture);
			shader->setInteger("u_usingTexture", 1);
		}
		else
		{
			Graphics::Texture t;
			shader->setTexture("u_Texture", t);
		}
		defaults = 0;
	}
	else
	{
		shader = new Graphics::Shader("res/shaders/default.shader");
		defaults = 1;
	}
}

Object::Object(Graphics::Mesh& m, Graphics::Shader& s, Graphics::Texture& t, glm::vec3 pos)
	:mesh(&m), shader(&s), texture(&t)
{constructor(pos);}
Object::Object(Graphics::Mesh& m, Graphics::Shader& s, glm::vec3 pos)
	:mesh(&m), shader(&s), texture(0)
{constructor(pos);}
Object::Object(Graphics::Mesh& m, glm::vec3 pos)
	:mesh(&m), shader(0), texture(0)
{constructor(pos);}

Object::Object(Graphics::Mesh::PreMade options, Graphics::Shader& s, Graphics::Texture& t, glm::vec3 pos)
	:shader(&s), texture(&t)
{
	mesh = new Graphics::Mesh(options);
	constructor();
	defaults += 2;
}
Object::Object(Graphics::Mesh::PreMade options, Graphics::Shader& s, glm::vec3 pos)
	:shader(&s), texture(0)
{
	mesh = new Graphics::Mesh(options);
	constructor();
	defaults += 2;
}
Object::Object(Graphics::Mesh::PreMade options, glm::vec3 pos)
	:shader(0), texture(0)
{
	mesh = new Graphics::Mesh(options);
	constructor();
	defaults += 2;
}

Object::Object(glm::vec3 pos)
	:mesh(0), shader(0), texture(0)
{
	transform.position = pos;
	transform.orientation = glm::vec3(0, 0, 1);
	hasShadow = true;
	isActive = 1;
	defaults = 0;
}

Object::~Object()
{
	if (defaults==1||defaults==3)
		delete shader;
	if (defaults == 2 || defaults == 3)
		delete mesh;
}

void Object::bind() const
{
	if(shader)
		shader->bind();
	if (texture)
		texture->bind(0);
	else
		Graphics::Texture::unBind(0);
	
	mesh->bind();
}
void Object::unBind() const
{
	if(mesh)
		mesh->unBind();
	if(shader)
		shader->unBind();
	if(texture)
		texture->unBind();
}

void Object::setShader(Graphics::Shader& newShader)
{
	if (defaults&1) {
		delete shader;
		defaults--;
	}
	shader = &newShader;
}

void Object::removeShader()
{
	if (!(defaults&1)) {
		shader = new Graphics::Shader("res/shaders/default.shader");
		defaults++;
	}
}

void Object::setTexture(Graphics::Texture& newTexture)
{
	texture = &newTexture;
	shader->bind();
	shader->setTexture("u_Texture", *texture);
	shader->setInteger("u_usingTexture", 1);
}

void Object::removeTexture()
{
	texture = 0;
	shader->bind();
	Graphics::Texture t;
	shader->setTexture("u_Texture", t);
	shader->setInteger("u_usingTexture", 0);
}

void Object::setMesh(Graphics::Mesh& newMesh)
{
	if (defaults&2) {
		delete mesh;
		defaults -= 2;
	}
	mesh = &newMesh;
}

void Object::removeMesh()
{
	if (defaults&2) {
		delete mesh;
		defaults -= 2;
	}
	mesh = 0;
}

void Object::setShadow(bool shadow)
{
	hasShadow = shadow;
}

void Object::setCameraView(const glm::mat4& proj,const glm::mat4& camView) const
{
	glm::mat4 MVP = proj * camView * transform.view();
	shader->setMatrix("u_Model", transform.view());
	shader->setMatrix("u_MVP", MVP);
}

Transform& Object::getTransform() 
{
	return transform;
}
